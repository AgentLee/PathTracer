#include "kdtree.h"
#include <QTime>

// KDAccelNodes represent leaves and interior nodes.
struct KDAccelNode
{
    void InitLeaf(int *primNums, int np, std::vector<int> *primitiveIndices)
    {
//        // Set to 3 so that we know it's a leaf node during bit operations.
//        flags = 3;

//        // np << 2 --> shift bits by 2 to the left and fill with 0s
//        // nPrims = nPrims | (np << 2)
//        nPrims |= (np << 2);

//        // Store primitive ids for leaf node
//        if(np == 0) {
//            onePrimitive = 0;
//        }
//        else if(np == 1) {
//            onePrimitive = primNums[0];
//        }
//        else {
//            primitiveIndicesOffset = primitiveIndices->size();

//            for(int i = 0; i < np; i++) {
//                primitiveIndices->push_back(primNums[i]);
//            }
//        }

        flags = 3;
        nPrims |= (np << 2);
        // Store primitive ids for leaf node
        if (np == 0)
            onePrimitive = 0;
        else if (np == 1)
            onePrimitive = primNums[0];
        else {
            primitiveIndicesOffset = primitiveIndices->size();
            for (int i = 0; i < np; ++i) primitiveIndices->push_back(primNums[i]);
        }
    }

    void InitInterior(int axis, int ac, float s)
    {
//        split = s;
//        flags = axis;
//        aboveChild |= (ac << 2);

        split = s;
        flags = axis;
        aboveChild |= (ac << 2);
    }

    // Getters
    float SplitPos() const { return split; }

    // >> shifts right and extends the top bit to preserve sign.
    // -0000 0000 0000 1000 (-8)
    // 1111 1111 1111 0111  (-8) shift 3 spaces
    // --------------------
    // 1111 1111 1111 1110
    int nPrimitives() const { return nPrims >> 2; }

    // flags = 0 (x)
    // 0000 0000 0000 0000
    //                   &
    // 0000 0000 0000 0011
    // -----------------------
    // 0000 0000 0000 0000 --> Return 0 for X
    //
    // flags = 1
    // 0000 0000 0000 0001
    //                   &
    // 0000 0000 0000 0011
    // -----------------------
    // 0000 0000 0000 0001 --> Return 1 for Y
    //
    // flags = 2
    // 0000 0000 0000 0010
    //                   &
    // 0000 0000 0000 0011
    // -----------------------
    // 0000 0000 0000 0010 --> Retunr 2 for Z
    int SplitAxis() const { return flags & 3; }

    // This will only return true when flags = 3.
    // flags gets set to 3 whenever a leaf node is initialized.
    // flags = 3
    // 0000 0000 0000 0011
    //                   &
    // 0000 0000 0000 0011
    // -------------------
    // 0000 0000 0000 0011 --> true (Leaf)
    //
    // flags = 2
    // 0000 0000 0000 0010
    //                   &
    // 0000 0000 0000 0011
    // -------------------
    // 0000 0000 0000 0010 --> false (interior)
    //
    // flags = 1
    // 0000 0000 0000 0001
    //                   &
    // 0000 0000 0000 0011
    // -------------------
    // 0000 0000 0000 0000 --> false (interior)
    //
    // flags = 0
    // 0000 0000 0000 0000
    //                   &
    // 0000 0000 0000 0011
    // -------------------
    // 0000 0000 0000 0000 --> false (interior)
    bool IsLeaf() const { return (flags & 3) == 3; }

    //
    int AboveChild() const { return aboveChild >> 2; }

    union
    {
        // Interior node
        float split;
        // Leaf
        int onePrimitive;
        // Leaf
        int primitiveIndicesOffset;
    };

    union
    {
        // Both
        // Use this to differentiate between interior nodes with x, y, z splits and leaf nodes
        int flags;
        // Leaf
        int nPrims;
        // Interior
        int aboveChild;
    };
};

enum class EdgeType { Start, End };
struct BoundEdge
{
    BoundEdge() {}
    BoundEdge(Float t, int primNum, bool starting) : t(t), primNum(primNum) {
//        type = starting ? EdgeType::Start : EdgeType::End;
        type = starting ? EdgeType::Start : EdgeType::End;
    }

    float t;
    int primNum;
    EdgeType type;
};

struct KDToDo
{
    const KDAccelNode *node;
    float tMin;
    float tMax;
};

KDTreeAccel::~KDTreeAccel()
{
    delete [] nodes;
}

KDTreeAccel::KDTreeAccel(const std::vector<std::shared_ptr<Primitive>> &p,
                        int isectCost, int traversalCost, float emptyBonus,
                        int maxPrims, int maxDepth) :
    isectCost(isectCost), traversalCost(traversalCost),
    emptyBonus(emptyBonus), maxPrims(maxPrims), primitives(p)
{

#define DEBUG
#ifndef DEBUG
    for(int i = 0; i < primitives.size(); i++) {
        std::cout << primitives.at(i)->name.toStdString() << std::endl;
    }
#endif

    QTime timer = QTime();
    timer.start();

    nextFreeNode = nAllocedNodes = 0;

    // If a maximum depth wasn't given in the constructor we have to set one
    if(maxDepth <= 0) {
        // PBRT says that 8 + 1.3logN is reasonable for max depth so the
        // tree can still grow in bad cases.
        maxDepth = std::round(8 + 1.3f * int(std::log(int64_t(primitives.size()))));
    }

    // ----------------------------------------------------------
    // Compute bounds for KD Tree construction
    // ----------------------------------------------------------
    // Want to store all the bounding boxes of the different primitives in the scene.
    // Doing so before the building the kd-tree will make it more efficient since
    // we won't have to do a bunch of WorldBound() calls in the recursion.
    std::vector<Bounds3f> primBounds;
    primBounds.reserve(primitives.size());

    for (const std::shared_ptr<Primitive> &prim : primitives) {
        // Get the bounds of the shape
        Bounds3f b = prim->shape->WorldBound();
        // Reset the bounds of the entire scene with each primitive
        bounds = Union(bounds, b);
        // Add to the bounds list
        primBounds.push_back(b);
    }

    // ----------------------------------------------------------
    // Allocate working memeory for KD Tree
    // ----------------------------------------------------------
    std::unique_ptr<BoundEdge[]> edges[3];
    for (int i = 0; i < 3; i++) {
        edges[i].reset(new BoundEdge[2 * primitives.size()]);
    }

    std::unique_ptr<int[]> prims0(new int[primitives.size()]);
    std::unique_ptr<int[]> prims1(new int[(maxDepth + 1) * primitives.size()]);

    // ----------------------------------------------------------
    // Initialize primNums for KD Tree
    // ----------------------------------------------------------
    // buildTree() has aa parameter that holds an array of primitive indices.
    // Because we're starting at the root, where all the primitives overlap
    // the root, we have to loop through all the primitives in the scene
    // and assign them a number.
    std::unique_ptr<int[]> primNums(new int[primitives.size()]);
    for(size_t i = 0; i < primitives.size(); i++) {
        primNums[i] = i;
    }

    // ----------------------------------------------------------
    // Start recursive construction
    // ----------------------------------------------------------
    // Start by passing in:
    //          - nodeNum           = 0
    //              Start with the first node in the array
    //
    //          - nodeBounds        = bounds
    //              The region that the node covers (entire bounds of the scene)
    //
    //          - allPrimBounds     = primBounds
    //              The list of each primitive's bounds
    //
    //          - primNums          = primNums
    //              The indices that overlap the current node (all of them)
    //
    //          - nPrimitives       = primitives.size()
    //              The number of primitives in the scene
    //
    //          - depth             = maxDepth
    //              Depth set at the beginning of the constructor
    //
    //          - edges             = edges
    //          - prims0            = prims0.get()
    //          - prims1            = prims1.get()
    //
    //          - badRefines        = set to 0 in constructor
    //              Number of split retries that didn't work
    buildTree(0, bounds, primBounds, primNums.get(), primitives.size(),
              maxDepth, edges, prims0.get(), prims1.get());

    int elapsed = timer.elapsed();
    std::cout << "kd-tree built in " << elapsed << std::endl;
}

// buildTree() gets called for each tree node. Determines if the node is a leaf or an interior node
// and updates the tree accordingly.
void KDTreeAccel::buildTree(int nodeNum, const Bounds3f &nodeBounds, const std::vector<Bounds3f> &allPrimBounds,
                            int *primNums, int nPrimitives,
                            int depth, const std::unique_ptr<BoundEdge[]> edges[],
                            int *prims0, int *prims1, int badRefines)
{
    // ----------------------------------------------------------
    // Get next free node
    // ----------------------------------------------------------
    // If all the allocated nodes are used, the node memory gets reallocated.
    // The first pass nAllocedNodes = 0 so it will get initialized here.
    if(nextFreeNode == nAllocedNodes) {
        int nNewAllocNodes = std::max(2 * nAllocedNodes, 512);

        // CHECK THIS
        KDAccelNode *n = (KDAccelNode*)malloc(nNewAllocNodes * sizeof(KDAccelNode));

        if(nAllocedNodes > 0) {
            // Copy over the nodes to temp n and delete whatever's in nodes
            memcpy(n, nodes, nAllocedNodes * sizeof(KDAccelNode));
            delete [] nodes;
        }

        // Reassign nodes to n since it now has doubled the memory
        nodes = n;
        // Set the number of allocated nodes to the doubled number
        nAllocedNodes = nNewAllocNodes;
    }

    ++nextFreeNode;

    // ----------------------------------------------------------
    // Initialize leaf node
    // ----------------------------------------------------------
    // If a region has a sufficiently small amount of primitives (<= maxPrims = 1)
    // or if we reached the depth value then the recursion stops and a leaf node is created.
    if(nPrimitives <= maxPrims || depth == 0) {
        nodes[nodeNum].InitLeaf(primNums, nPrimitives, &primitiveIndices);

        return;
    }

    // We have to classify the primitives wrt the splitting axis.
    // Choose split axis position for interior node using the SAH.
    // The cost is computed for a series of splitting planes in the node.
    // The split with the lowest cost gets chosen.
    // isectCost = 80
    // traversalCost = 1

    // SAH in BVH vs kd-tree
    // The larger the ratio between costs shows that a traversal through
    // a kd-tree is less expensive than traversing through a BVH.
    // You also want to consider splitting when a child has no primitives.

    // ----------------------------------------------------------
    // Initialize interior node
    // ----------------------------------------------------------
    int bestAxis        = -1;
    int bestOffset      = -1;
    float bestCost      = INFINITY;
    float oldCost       = isectCost * Float(nPrimitives);
    float totalSA       = nodeBounds.SurfaceArea();
    float invTotalSA    = 1 / totalSA;
    Vector3f d          = nodeBounds.max - nodeBounds.min;

    // Choose axis to split along
    int axis    = nodeBounds.MaximumExtent();
    int retries = 0;

retrySplit:

    // Initialize edges for axis
    for(int i = 0; i < nPrimitives; i++) {
        int pn = primNums[i];
        const Bounds3f &bounds = allPrimBounds[pn];
        edges[axis][2 * i]      = BoundEdge(bounds.min[axis], pn, true);
        edges[axis][2 * i + 1]  = BoundEdge(bounds.max[axis], pn, false);
    }

    // Sort edges for axis
    // Want to take the edges array from a0,a1,b0,b1,c0,c1
    // and turn it into a0,b0,a1,b1,c0,c1
    // ----|-----------|
    //     a0          a1
    // -------------|--------------|--------------------
    //             b0             b1
    // -------------------------------------|----------|
    //                                      c0         c1
    // ----|--------|--|-----------|--------|----------|
    //     a0      b0  a1         b1        c0         c1

    std::sort(&edges[axis][0], &edges[axis][2 * nPrimitives],
            [](const BoundEdge &e0, const BoundEdge &e1) -> bool
    {
        // Since the min/max values match we have to
        // sort based on their type (start/end).
        if(e0.t == e1.t) {
           return (int)e0.type < (int)e1.type;
        }
        else {
           return e0.t < e1.t;
        }
    });

    for(int i = 0; i < nPrimitives * 2; i++) {
        float t = edges[axis][i].t;
//        std::cout << t << std::endl;
    }

    // Compute cost for all splits to find best
    int nBelow = 0;
    int nAbove = nPrimitives;
    for(int i = 0; i < 2 * nPrimitives; i++) {
        if(edges[axis][i].type == EdgeType::End) {
            --nAbove;
        }

        float edgeT = edges[axis][i].t;

        // Compute cost for the split
        if(edgeT > nodeBounds.min[axis] &&
           edgeT < nodeBounds.max[axis]) {
            // Compute child surface areas for split at edgeT
            int otherAxis0 = (axis + 1) % 3;
            int otherAxis1 = (axis + 2) % 3;

            float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                 (edgeT - nodeBounds.min[axis]) *
                                     (d[otherAxis0] + d[otherAxis1]));
            float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                 (nodeBounds.max[axis] - edgeT) *
                                     (d[otherAxis0] + d[otherAxis1]));

            float pBelow = belowSA * invTotalSA;
            float pAbove = aboveSA * invTotalSA;
            float eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0;
            float cost = traversalCost + isectCost * (1 - eb) * (pBelow * nBelow + pAbove * nAbove);

            // Update best split if this is lowest cost so far
            if(cost < bestCost) {
                bestCost = cost;
                bestAxis = axis;
                bestOffset = i;
            }
        }

        if (edges[axis][i].type == EdgeType::Start) {
            ++nBelow;
        }
    }
    assert(nBelow == nPrimitives && nAbove == 0);

    // Create leaf if no good splits were found
    // retries can only work if less than 2 (since there are 3 axes)
    if(bestAxis == -1 && retries < 2) {
        ++retries;
        axis = (axis + 1) % 3;
        goto retrySplit;
    }

    if(bestCost > oldCost) {
        ++badRefines;
    }

    if((bestCost > 4 * oldCost && nPrimitives < 16) || bestAxis == -1 ||
            badRefines == 3) {
        nodes[nodeNum].InitLeaf(primNums, nPrimitives, &primitiveIndices);
        return;
    }

    // Classify primitives with respect to split
    int n0 = 0;
    int n1 = 0;

    for(int i = 0; i < bestOffset; i++) {
        if(edges[bestAxis][i].type == EdgeType::Start) {
            prims0[n0++] = edges[bestAxis][i].primNum;
        }
    }

    for(int i = bestOffset + 1; i < 2 * nPrimitives; i++) {
        if(edges[bestAxis][i].type == EdgeType::End) {
            prims1[n1++] = edges[bestAxis][i].primNum;
        }
    }

    // Recursively initialize children nodes
    float tSplit = edges[bestAxis][bestOffset].t;

    Bounds3f bounds0 = nodeBounds;
    Bounds3f bounds1 = nodeBounds;
    bounds0.max[bestAxis] = bounds1.min[bestAxis] = tSplit;

    buildTree(nodeNum + 1, bounds0, allPrimBounds, prims0, n0,
              depth - 1, edges, prims0, prims1 + nPrimitives, badRefines);

    int aboveChild = nextFreeNode;
    nodes[nodeNum].InitInterior(bestAxis, aboveChild, tSplit);

    buildTree(aboveChild, bounds1, allPrimBounds, prims1, n1,
              depth - 1, edges, prims0, prims1 + nPrimitives, badRefines);
}

bool KDTreeAccel::Intersect(const Ray &r, Intersection *isect) const
{
    // ----------------------------------------------------------
    // Compute initial parametric range of ray inside KD tree extent
    // ----------------------------------------------------------
    float tMin;
    float tMax;

    // If the ray misses the overall bounds of the tree then we can just return false.
    bool intersects = bounds.IntersectP(r, &tMin, &tMax);
    if(!intersects) {
        return false;
    }

    // ----------------------------------------------------------
    // Prepare to traverse KD tree for ray
    // ----------------------------------------------------------
    Vector3f invDir(1 / r.direction.x, 1 / r.direction.y, 1 / r.direction.z);
    constexpr int maxTodo = 64;
    KDToDo todo[maxTodo];
    int todoPos = 0;

    // ----------------------------------------------------------
    // Traverse KD tree nodes in order for ray
    // ----------------------------------------------------------
    // Set to false since we technically didn't hit anything yet
    bool hit = false;
    // Set to the first node to begin traversal
    const KDAccelNode *node = &nodes[0];

    Point3f isectPoint(0.f);

    while(node != nullptr) {
        // Bail out if we found a hit closer than the current node
        if(r.tMax < tMin) {
            break;
        }

        // Interior node
        if(!node->IsLeaf()) {
            /// Compute parametric distance along ray to split plane
            // Get the split axis for the node
            int axis        = node->SplitAxis();
            float tPlane    = (node->SplitPos() - r.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const KDAccelNode *firstChild;
            const KDAccelNode *secondChild;

            int belowFirst = (r.origin[axis] < node->SplitPos()) ||
                             (r.origin[axis] == node->SplitPos() && r.direction[axis] <= 0);

            if(belowFirst) {
                firstChild = node + 1;
                secondChild = &nodes[node->AboveChild()];
            }
            else {
                firstChild = &nodes[node->AboveChild()];
                secondChild = node + 1;
            }

            // Advance to next child node and possibly queue another child
            if(tPlane > tMax || tPlane <= 0) {
                node = firstChild;
            }
            else if(tPlane < tMin) {
                node = secondChild;
            }
            else {
                // Queue secondChild in todo list
                todo[todoPos].node = secondChild;
                todo[todoPos].tMin = tPlane;
                todo[todoPos].tMax = tMax;
                ++todoPos;

                node = firstChild;
                tMax = tPlane;
            }
        }
        // Leaf node
        else {
            // Check for intersections inside leaf node
            int nPrimitives = node->nPrimitives();
            if(nPrimitives == 1) {
                const std::shared_ptr<Primitive> &p = primitives[node->onePrimitive];

                bool intersects = p->Intersect(r, isect);
                if(intersects) {
//                    hit = true;
                    // First hit
//                    if(!hit) {
//                        isectPoint = isect->point;
//                        hit = true;
//                    }
//                    else {
//                        float rToIsect      = glm::distance(r.origin, isect->point);
//                        float rToPrevIsect  = glm::distance(r.origin, isectPoint);

//                        if(rToIsect < rToPrevIsect) {
//                            isectPoint = isect->point;
//                            hit = true;
//                        }
//                        else {
//                            hit = false;
//                        }
//                    }
                    hit = true;
                } else {
                    hit = false;
                }
            }
            else {
                for(int i = 0; i < nPrimitives; i++) {
                    int index = primitiveIndices[node->primitiveIndicesOffset + i];
                    const std::shared_ptr<Primitive> &p = primitives[index];

                    bool intersects = p->Intersect(r, isect);
                    if(intersects) {
                        // First hit
//                        if(!hit) {
//                            isectPoint = isect->point;
//                            hit = true;
//                        }
//                        else {
//                            float rToIsect      = glm::distance(r.origin, isect->point);
//                            float rToPrevIsect  = glm::distance(r.origin, isectPoint);

//                            if(rToIsect < rToPrevIsect) {
//                                isectPoint = isect->point;
//                                hit = true;
//                            }
//                            else {
//                                hit = false;
//                            }
//                        }
                        hit = true;
                    } else {
                        hit = false;
                    }
                }
            }

            // Grab next node to process from todo list
            if(todoPos > 0) {
                --todoPos;
                node = todo[todoPos].node;
                tMin = todo[todoPos].tMin;
                tMax = todo[todoPos].tMax;
            }
            else {
                break;
            }
        }
    }

    return (hit && IntersectP(r, isect));
}

bool KDTreeAccel::IntersectP(const Ray &r, Intersection *isect) const
{
     // Compute initial parametric range of ray inside kd-tree extent
     Float tMin;
     Float tMax;

     bool intersects = bounds.IntersectP(r, &tMin, &tMax);
     if (!intersects) {
         return false;
     }

     // Prepare to traverse kd-tree for ray
     Vector3f invDir(1 / r.direction.x, 1 / r.direction.y, 1 / r.direction.z);
     constexpr int maxTodo = 64;
     KDToDo todo[maxTodo];
     int todoPos = 0;

     bool hit = false;
     const KDAccelNode *node = &nodes[0];

     while (node != nullptr) {
         if(r.tMax < tMin) {
             break;
         }

         if (node->IsLeaf()) {
             // Check for shadow ray intersections inside leaf node
             int nPrimitives = node->nPrimitives();
             if (nPrimitives == 1) {
                 const std::shared_ptr<Primitive> &p = primitives[node->onePrimitive];

                 if (p->Intersect(r, isect)) {
                     return true;
                 }
             } else {
                 for (int i = 0; i < nPrimitives; ++i) {
                     int primitiveIndex = primitiveIndices[node->primitiveIndicesOffset + i];
                     const std::shared_ptr<Primitive> &prim = primitives[primitiveIndex];

                     if (prim->Intersect(r, isect)) {
                         return true;
                     }
                 }
             }

             // Grab next node to process from todo list
             if (todoPos > 0) {
                 --todoPos;
                 node = todo[todoPos].node;
                 tMin = todo[todoPos].tMin;
                 tMax = todo[todoPos].tMax;
             } else
                 break;
         }
         else {
             // Process kd-tree interior node

             // Compute parametric distance along ray to split plane
             int axis = node->SplitAxis();
             Float tPlane = (node->SplitPos() - r.origin[axis]) * invDir[axis];

             // Get node children pointers for ray
             const KDAccelNode *firstChild, *secondChild;
             int belowFirst =
                 (r.origin[axis] < node->SplitPos()) ||
                 (r.origin[axis] == node->SplitPos() && r.direction[axis] <= 0);
             if (belowFirst) {
                 firstChild = node + 1;
                 secondChild = &nodes[node->AboveChild()];
             } else {
                 firstChild = &nodes[node->AboveChild()];
                 secondChild = node + 1;
             }

             // Advance to next child node, possibly enqueue other child
             if (tPlane > tMax || tPlane <= 0)
                 node = firstChild;
             else if (tPlane < tMin)
                 node = secondChild;
             else {
                 // Enqueue _secondChild_ in todo list
                 todo[todoPos].node = secondChild;
                 todo[todoPos].tMin = tPlane;
                 todo[todoPos].tMax = tMax;
                 ++todoPos;

                 node = firstChild;
                 tMax = tPlane;
             }
         }
     }
     return false;
}
