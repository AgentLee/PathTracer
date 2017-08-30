#pragma once

#include "geometry/primitive.h"

struct KDAccelNode;
struct BoundEdge;
struct KDToDo;

class KDTreeAccel : public Primitive
{
    // Functions
public:
    KDTreeAccel(const std::vector<std::shared_ptr<Primitive>> &p,
                int isectCost = 80, int traversalCost = 1,
                Float emptyBonus = 0.5, int maxPrims = 1, int maxDepth = -1);
    ~KDTreeAccel();

    void buildTree(int nodeNum, const Bounds3f &nodeBounds,
                   const std::vector<Bounds3f> &allPrimBounds, int *primNums,
                   int nPrimitives, int depth,
                   const std::unique_ptr<BoundEdge[]> edges[3],
                   int *prims0, int *prims1, int badRefines = 0);

    bool Intersect(const Ray &r, Intersection *isect) const;
    bool IntersectP(const Ray &r, Intersection *isect) const;

private:
    // Members
    const int isectCost;
    const int traversalCost;
    const int maxPrims;
    const float emptyBonus;
    std::vector<std::shared_ptr<Primitive>> primitives;
    // If more than one primitive overlaps a leaf node then the indices get stored here.
    std::vector<int> primitiveIndices;

    KDAccelNode *nodes;
    // If both of these variables are set to 0 and not allocating nodes at the start,
    // allocation will be done immediately when the first node is initialized.
    // Records the next node in the array that is available
    int nextFreeNode;
    // Records the total number of nodes that have been allocated
    int nAllocedNodes;

    Bounds3f bounds;
};
