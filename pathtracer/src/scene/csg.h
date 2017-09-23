#pragma once

#include "geometry/primitive.h"

class CSGNode
{
public:
    CSGNode();
    CSGNode(std::shared_ptr<Primitive> p);
    CSGNode(int op);
    ~CSGNode();

    CSGNode *left;
    CSGNode *right;

    std::shared_ptr<Primitive> prim;
    int operation;
};


// Constructive Solid Geometry
class CSG : public Primitive
{
public:
    // Constructors
    CSG();
    CSG(const std::vector<std::shared_ptr<Primitive>> &p);
    // Destructors
    ~CSG();

    // The root is going to have the combined primitives
    CSGNode *root;

    bool Intersect(CSGNode *T, const Ray &r, Intersection *isect) const;
    bool unionOp(CSGNode *left, CSGNode *right,  const Ray &r, Intersection *isect) const;
    bool diffOp(CSGNode *left, CSGNode *right, const Ray &r, Intersection *isect) const;
    bool intersectOp(CSGNode *left, CSGNode *right,  const Ray &r, Intersection *isect) const;
};
