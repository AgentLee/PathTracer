#include "bvh.h"
#include <QTime>
// Feel free to ignore these structs entirely!
// They are here if you want to implement any of PBRT's
// methods for BVH construction.

struct BucketInfo
{
    int count = 0;
    Bounds3f bounds;
};

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
        : primitiveNumber(primitiveNumber),
          bounds(bounds),
          centroid(.5f * bounds.min + .5f * bounds.max) {}
    int primitiveNumber;
    Bounds3f bounds;
    Point3f centroid;
};

// Represents a BVH node
struct BVHBuildNode {
    // BVHBuildNode Public Methods
    void InitLeaf(int first, int n, const Bounds3f &b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }

    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = Union(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }

    // Bounds of all the children of the node
    Bounds3f bounds;
    // Pointers to its two children
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
};

struct MortonPrimitive {
    int primitiveIndex;
    unsigned int mortonCode;
};

struct LBVHTreelet {
    int startIndex, nPrimitives;
    BVHBuildNode *buildNodes;
};

struct LinearBVHNode {
    Bounds3f bounds;
    union {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    unsigned short nPrimitives;  // 0 -> interior node, 16 bytes
    unsigned char axis;          // interior node: xyz, 8 bytes
    unsigned char pad[1];        // ensure 32 byte total size
};

BVHAccel::~BVHAccel()
{
    delete [] nodes;
}

// Constructs an array of BVHPrimitiveInfos, recursively builds a node-based BVH
// from the information, then optimizes the memory of the BVH
BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive> > &p, int maxPrimsInNode)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p)
{
     QTime timer = QTime();
     timer.start();

    //TODO

    // 1. Compute information about each primitive and store in an array when making a tree
    // 2. Build the tree recursively. Each node holds pointers to children and
    //    each leaf node holds references to primitives.
    // 3. Convert the tree to a compact and more efficient (no pointers) representation for rendering.

    if(primitives.size() == 0) {
        return;
    }

    /// Initialize primitiveInfo array for primitives
    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    // Store the centroid of the the bounding box, the bounding box, and the index in the array
    for(size_t i = 0; i < primitives.size(); i++) {
        primitiveInfo[i] = { i, primitives[i].get()->shape->WorldBound() };
    }

    /// Build BVH tree for primitives using primitiveInfo
    // In the initial calls, pass all of the primitives to be stored in the tree.
    // Return a pointer to the root of the tree (BVHBuildNode).
    int totalNodes = 0;

    // A new array of pointers to primitives is return through orderedPrims.
    // This stores the primitives ordered so that the primitives in leaf nodes
    // are contiguous in the array. Once the tree is built, you swap the original
    // with the ordered primitives.
    std::vector<std::shared_ptr<Primitive>> orderedPrims;

    // Build the tree
    root = recursiveBuild(primitiveInfo, 0, primitives.size(), &totalNodes, orderedPrims);
    // Swap for rendering
    primitives.swap(orderedPrims);

    nodes = (LinearBVHNode*)malloc(totalNodes * sizeof*nodes);
    int offset = 0;
    flattenBVHTree(root, &offset);



    /// Compute representation of DFS of BVH tree
    // Pointer or no pointer?
    int elapsed = timer.elapsed();
    std::cout << "BVH Tree built in " << elapsed << std::endl;
}

int BVHAccel::flattenBVHTree(BVHBuildNode *node, int *offset)
{
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;

    int myOffset = (*offset)++;

    if(node->nPrimitives > 0) {
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    } else {
        /// Create interior flattened BVH node
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;

        flattenBVHTree(node->children[0], offset);

        linearNode->secondChildOffset = flattenBVHTree(node->children[1], offset);
    }

    return myOffset;
}

bool BVHAccel::Intersect(const Ray &ray, Intersection *isect) const
{
    bool hit = false;

    Vector3f invDir(1 / ray.direction.x, 1 / ray.direction.y, 1 / ray.direction.z);

    int dirIsNeg[3] = {  invDir.x < 0, invDir.y < 0, invDir.z < 0 };

    /// Follow the ray through the BVH nodes to find intersections with the primitives
    // Holds the offset to the next free element in the stack
    int toVisitOffset = 0;
    // Holds the offset into the nodes array of the node to be visited.
    int currentNodeIndex = 0;
    // Remaining nodes that need to be visited. (Acts like the stack)
    int nodesToVisit[64];

    Intersection minIntersection = Intersection();
    float tMin = 1000000.f;

    while(true)
    {
        const LinearBVHNode *node = &nodes[currentNodeIndex];

        /// Check ray against BVH node
        // Need to check if the ray hits the node's bounding box or
        // if the ray's origin is inside of the bounding box.
        // In either case we need to determine if it's a leaf node.
        // If it's a leaf node then we check the ray against the primitives.
        // Otherwise we process the children.
        // If there is no intersection then we get the offset to visit
        // the next node in the array.

        // If we're at a leaf node then we loop through all of its primitives
        // Loop through all primitives to find the one with the smallest t value.

        // If we're not at a leaf node then we have to visit both of the children.
        // Visit the first child before the second child. This allows the t value
        // to be updated in the case the ray hits a primitive in the first child.

        // If there is a negative t value, visit the second child before the first child.
        // In this case you need to check if the ray's origin is in the bounding box.

        // Check to see if the ray intersects the node's bounding box or if we're in the box
        // If we do hit the node then we test the intersection with its primitives.
        // No intersection results in getting the next offset to visit. This case can also
        // be that the stack is empty.
        if(node->bounds.IntersectP(ray, invDir, dirIsNeg)) {
            if(node->nPrimitives > 0) {
                /// Intersect ray with primitives in leaf node
                // The ray has to be tested for intersection with primitives inside.
                // We can find the next node to visit from the stack.
                for(int i = 0; i < node->nPrimitives; i++) {

                    Intersection _isect = Intersection();
                    if(primitives.at(node->primitivesOffset + i)->Intersect(ray, &_isect)) {

                        if(_isect.t < tMin) {
                            tMin = _isect.t;
                            minIntersection = _isect;
                            *isect = minIntersection;

                            hit = true;
                        }
                    }
                }

                if(toVisitOffset == 0) {
                    break;
                }

                currentNodeIndex = nodesToVisit[--toVisitOffset];
            } else {
                /// Put far BVHnode on nodesToVisit stack and advance to the near node
                if(dirIsNeg[node->axis]) {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->secondChildOffset;
                } else {
                    nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }
            }
        } else {
            if(toVisitOffset == 0) {
                break;
            }

            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }

    return hit;
}

// Return a BVH for subsequent primitives from primitiveInfo.
// @totalNodes keeps track of the number of nodes created from this
// so that the LinearBVHNodes can be created.
// @orderedPrims stores references to primitives as leaf nodes are created.
BVHBuildNode* BVHAccel::recursiveBuild(
    std::vector<BVHPrimitiveInfo> &primitiveInfo,
    int start, int end, int *totalNodes,
    std::vector<std::shared_ptr<Primitive>> &orderedPrims)
{
    // Create a new node
    BVHBuildNode *node = new BVHBuildNode();
    // Increment node count
    (*totalNodes)++;

    /// Compute the bounds of all the primitives
    Bounds3f bounds;
    for(int i = start; i < end; i++) {
        // Use union to get the bounds of the current primitive and the overall
        bounds = Union(bounds, primitiveInfo[i].bounds);
    }

    // Determine if we should make a leaf node
    int nPrimitives = end - start;
    // At leaf node
    if(nPrimitives == 1) {
        /// Create leaf node
        // Primitives that overlap the leaf node are added to orderedPrims
        int firstPrimOffset = orderedPrims.size();

        for(int i = start; i < end; i++) {
            int primNum = primitiveInfo[i].primitiveNumber;

            orderedPrims.push_back(primitives[primNum]);
        }

        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);

        return node;
    }
    // At interior node
    // We want to divide into 2 subtrees. The general goal is to
    // pick a partition of primitives that don't overlap as much.
    else {
        /// Get the bounds around the centroids and see where to split
        Bounds3f centroidBounds;
        for(int i = start; i < end; i++) {
            centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
        }

        int dim = centroidBounds.MaximumExtent();

        /// Partition primitives into two sets and build children
        int mid = (start + end) / 2;
        if(centroidBounds.max[dim] == centroidBounds.min[dim]) {
            /// Create leaf node
            int firstPrimOffset = orderedPrims.size();

            for(int i = start; i < end; i++) {
                int primNum = primitiveInfo[i].primitiveNumber;

                orderedPrims.push_back(primitives[primNum]);
            }

            node->InitLeaf(firstPrimOffset, nPrimitives, bounds);

            return node;
        } else {
            /// Partition primitives using the surface area heuristic
            // The partition along the chosen axis that gives the minimal
            // SAH cost is found by considering a number of candidate partitions
            // After a certain number of iterations and the number of primitives
            // decreases, we partition to equally sized subsets

            if(nPrimitives <= 4) {
                /// Partition to equal subsets
                mid = (start + end) / 2;

                // This takes the start, middle, end pointer, and comparison function.
                // Need to look into this more.
                std::nth_element(&primitiveInfo[start],
                                 &primitiveInfo[mid],
                                 &primitiveInfo[end - 1] + 1,
                                 [dim](const BVHPrimitiveInfo &a, const BVHPrimitiveInfo &b)
                {
                    return a.centroid[dim] < b.centroid[dim];
                });
            } else {
                /// Allocate BucketInfo for SAH partition buckets
                int nBuckets = 12;

#define MAC     // Comment this out for Windows
#ifndef MAC
                BucketInfo buckets[nBuckets];
#else
                BucketInfo *buckets = new BucketInfo[nBuckets];
#endif
                /// Initialize BucketInfo for SAH partition buckets
                // Determine which bucket the centroid is in and
                // update the bucket's bounds to include the primitive's bounds
                for(int i = start; i < end; i++) {
                    int b = nBuckets * centroidBounds.Offset(primitiveInfo[i].centroid)[dim];

                    if(b == nBuckets) {
                        b = nBuckets - 1;
                    }

                    buckets[b].count++;
                    buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
                }

                /// Compute costs for splitting after each bucket
                // At this point we have the number of primitives and their bounds in each bucket
                // We want to find the SAH to estimate the the cost of spliting at each of the
                // bucket boundaries. We loop through all of the buckets and initialize the cost
                // to store the estimated SAH for splitting after i.
                float cost[nBuckets - 1];

                for(int i = 0; i < nBuckets - 1; i++) {
                    Bounds3f b0;// = Bounds();
                    Bounds3f b1;// = Bounds();

                    int count0 = 0;
                    int count1 = 0;

                    for(int j = 0; j <= i; j++) {
                        b0 = Union(b0, buckets[j].bounds);

                        count0 += buckets[j].count;
                    }

                    for(int j = i + 1; j < nBuckets; j++) {
                        b1 = Union(b1, buckets[j].bounds);

                        count1 += buckets[j].count;
                    }

                    cost[i] = .125f + (count0 * b0.SurfaceArea() +
                                       count1 * b1.SurfaceArea()) / bounds.SurfaceArea();
                }

                /// Find bucket to split at that minimizes SAH metric
                // Scan through the cost array to find the partition with the minimum cost
                float minCost = cost[0];

                int minCostSplitBucket = 0;

                for(int i = 1; i < nBuckets - 1; i++) {
                    if(cost[i] < minCost) {
                        minCost = cost[i];

                        minCostSplitBucket = i;
                    }
                }

                /// Create leaf or split primitives in the SAH bucket
                float leafCost = nPrimitives;

                if(nPrimitives > maxPrimsInNode || minCost < leafCost) {
                    BVHPrimitiveInfo *pmid = std::partition(&primitiveInfo[start],
                                                            &primitiveInfo[end - 1] + 1,
                                                            [=](const BVHPrimitiveInfo &pi)
                                                            {
                                                                int b = nBuckets * centroidBounds.Offset(pi.centroid)[dim];

                                                                if(b == nBuckets) b = nBuckets - 1;

                                                                return b <= minCostSplitBucket;
                                                            });

                    mid = pmid - &primitiveInfo[0];
                } else {
                    /// Create leaf node
                    int offset = orderedPrims.size();
                    for(int i = start; i < end; i++) {
                        int primNum = primitiveInfo[i].primitiveNumber;

                        orderedPrims.push_back(primitives[primNum]);
                    }

                    node->InitLeaf(offset, nPrimitives, bounds);

                    return node;
                }
            }

            // Force the split
//            if(mid == start || mid == end) {
//                mid = (start + end) / 2;
//            }

            // Recursive calls
            node->InitInterior(dim,
                               recursiveBuild(primitiveInfo, start, mid, totalNodes, orderedPrims),
                               recursiveBuild(primitiveInfo, mid, end, totalNodes, orderedPrims));
        }
    }

    return node;
}
