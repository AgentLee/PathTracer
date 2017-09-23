#include "csg.h"

enum operation
{
    NO_OP       = 0,
    UNION       = 1,
    DIFF        = 2,
    INTERSECT   = 3
};

// ----------------------------------
// CSG Node
// ----------------------------------
CSGNode::CSGNode() :
    left(nullptr), right(nullptr), prim(nullptr), operation(NO_OP)
{

}

CSGNode::CSGNode(std::shared_ptr<Primitive> p) :
    left(nullptr), right(nullptr), prim(p), operation(NO_OP)
{

}

CSGNode::CSGNode(int op) :
    left(nullptr), right(nullptr), prim(nullptr), operation(op)
{

}

CSGNode::~CSGNode()
{

}


// ----------------------------------
// CSG
// ----------------------------------
CSG::CSG()
{
    root = new CSGNode();
}

CSG::CSG(const std::vector<std::shared_ptr<Primitive> > &p)
{
    std::cout << "Building CSG" << std::endl;

    // TEST ------------------------------------------------
    // Find the primitive that's marked with "csg"
    // Use "CSG_Test.json" and "CSG_CornellBox.json"
    // Don't take into account lights, walls, or floors.
    std::vector<std::shared_ptr<Primitive>> prims;
    for(int i = 0; i < p.size(); i++) {
        if(p.at(i)->material == nullptr) {
            continue;
        }

        if(!p.at(i)->csg) {
            continue;
        }

        prims.push_back(p.at(i));
    }

    // TODO
    // Handle more than two objects
    root        = new CSGNode(DIFF);
    root->left  = new CSGNode(prims.at(0));     // Cube
    root->right = new CSGNode(prims.at(1));     // Sphere

#define DEBUG
#ifndef DEBUG
    std::cout << root->left->prim->name.toStdString() << std::endl;
    std::cout << root->right->prim->name.toStdString() << std::endl;
#endif
}

CSG::~CSG()
{

}

bool CSG::Intersect(CSGNode *T, const Ray &r, Intersection *isect) const
{
    bool result = false;
    switch(T->operation)
    {
    case DIFF:
        result = diffOp(T->left, T->right, r, isect);
        break;
    case INTERSECT:
        result = intersectOp(T->left, T->right, r, isect);
        break;
    default:    // NO_OP or UNION
        break;
    }

    return result;
}

bool CSG::unionOp(CSGNode *left, CSGNode *right,  const Ray &r, Intersection *isect) const
{
    Intersection lIsx, rIsx;
    bool hitsLeftNode   = left->prim->Intersect(r, &lIsx);
    bool hitsRightNode  = right->prim->Intersect(r, &rIsx);

    if(hitsLeftNode || hitsRightNode) {
        // Set the intersection to the closer z value primitive
        if(lIsx.point.z < rIsx.point.z) {
            left->prim->Intersect(r, isect);
        } else {
            right->prim->Intersect(r, isect);
        }

        return true;
    }

    // If we reached here then we obviously have no intersection and return false
    *isect = Intersection();
    return false;
}

// Left - Right
// Cube - Sphere
bool CSG::diffOp(CSGNode *left, CSGNode *right, const Ray &r, Intersection *isect) const
{
    Intersection lIsx, rIsx;
    bool hitsLeftNode   = left->prim->Intersect(r, &lIsx);
    bool hitsRightNode  = right->prim->Intersect(r, &rIsx);

    // Hits Cube and not Sphere
    if(hitsLeftNode && !hitsRightNode) {
        // Set the intersection to the left primitive
        left->prim->Intersect(r, isect);

        return true;
    }

    // Hits Sphere and not Cube
    if(hitsRightNode && !hitsLeftNode) {
        // Considered not an intersection point for CSG
        // So reset the intersection and return false
        *isect = Intersection();

        return false;
    }

    // Hits Cube and Sphere
    if(hitsLeftNode && hitsRightNode) {
        // Ray march through the objects to see where the difference lies
        // For the Cube and Sphere: just see where the Sphere ends and use that
        // as the intersection point and cast a ray from that point to the cube.

        // Get the start point for left node (Cube)
        Point3f leftStart = lIsx.point;

        // New ray to find left end intersection
        Vector3f direction  = glm::normalize(r.direction);
        Point3f origin      = leftStart + (direction * 0.1f);
        Ray leftRay         = Ray(origin, direction);

        // Get the end point for the left node (Cube)
        Point3f leftEnd;
        bool hitsLeftEnd = left->prim->Intersect(leftRay, &lIsx);
        if(hitsLeftEnd) {
            leftEnd = lIsx.point;
        }

        // Get the start point for the right node (Sphere)
        Point3f rightStart = rIsx.point;

        // New ray to find right end intersection
        origin              = rightStart + (direction * 0.1f);
        Ray rightRay        = Ray(origin, direction);

        // Get the end point for the right node (Sphere)
        Point3f rightEnd;
        bool hitsRightEnd = right->prim->Intersect(rightRay, &rIsx);
        if(hitsRightEnd) {
            rightEnd = rIsx.point;
        }

        float t     = 0.f;
        float step  = 0.1f;

        // Make a temp ray
        Ray ray = r;

        // Ray march 1000 times or until we hit the outside of the cube
        for(int i = 0; i < 1000; i++) {
            // Get point on ray
            Vector3f d  = glm::normalize(ray.direction);
            Point3f p   = ray.origin + (d * t);

            // Make a new ray to see if it hits the objects using p
            Ray _ray_ = Ray(p, d);

            // Get intersection for left node
            Intersection _lIsx;
            bool _hitsLeftNode = left->prim->Intersect(_ray_, &_lIsx);

            // Get intersection for right node
            Intersection _rIsx;
            bool _hitsRightNode = right->prim->Intersect(_ray_, &_rIsx);

            // Hits left and not right (Cube and not Sphere)
            // We are passed the right object.
            // This is the intersection that we return true.
            if(_hitsLeftNode && !_hitsRightNode) {
                bool intersection = left->prim->Intersect(_ray_, isect);

                return true;
            }

            // Doesn't hit left and right (not Cube and not Sphere)
            // Basically left both objects and went too far.
            if(!_hitsLeftNode && !_hitsRightNode) {
                // Revert to when the t value hit the objects
                t -= step;
                // Make the step size smaller
                step /= 4.f;
            }

            // Doesn't hit the left node but hits the right (not Cube and Sphere)
            // This is the case where the right object can be poking out
            // of the left object if they're not interescting. In that case
            // we want to forget about any of those points.
            if(!_hitsLeftNode && _hitsRightNode) {
                // Do nothing
            }

            // Hits left and right (Cube and Sphere)
            // We still need to ray march through the objects
            // since we're either in just the right object or both left and right.
            if(_hitsLeftNode && _hitsRightNode) {
                // Do nothing
            }

            // Increment t for next step
            t += step;
        }
    }

    // If we reached here then we obviously have no intersection and return false
    *isect = Intersection();
    return false;
}

bool CSG::intersectOp(CSGNode *left, CSGNode *right, const Ray &r, Intersection *isect) const
{
    Intersection lIsx, rIsx;
    bool hitsLeftNode   = left->prim->Intersect(r, &lIsx);
    bool hitsRightNode  = right->prim->Intersect(r, &rIsx);

    // This is the only case we have to deal with for intersection.
    // The left and right nodes are both hit by the ray.
    if(hitsLeftNode && hitsRightNode) {
        // Ray march through the objects to see where the intersection lies

        // Get the start point for left node (Cube)
        Point3f leftStart = lIsx.point;

        // New ray to find left end intersection
        Vector3f direction  = glm::normalize(r.direction);
        Point3f origin      = leftStart + (direction * 0.1f);
        Ray leftRay         = Ray(origin, direction);

        // Get the end point for the left node (Cube)
        Point3f leftEnd;
        bool hitsLeftEnd = left->prim->Intersect(leftRay, &lIsx);
        if(hitsLeftEnd) {
            leftEnd = lIsx.point;
        }

        // Get the start point for the right node (Sphere)
        Point3f rightStart = rIsx.point;

        // New ray to find right end intersection
        origin              = rightStart + (direction * 0.1f);
        Ray rightRay        = Ray(origin, direction);

        // Get the end point for the right node (Sphere)
        Point3f rightEnd;
        bool hitsRightEnd = right->prim->Intersect(rightRay, &rIsx);
        if(hitsRightEnd) {
            rightEnd = rIsx.point;
        }

        float t     = 0.f;
        float step  = 0.04f;

        // Make a temp ray
        Ray ray = r;

        // Ray march 1000 times or until we hit the outside of the cube
        for(int i = 0; i < 1000; i++) {
            // Get point on ray
            Vector3f d  = glm::normalize(ray.direction);
            Point3f p   = ray.origin + (d * t);

            // Make a new ray to see if it hits the objects using p
            Ray _ray_ = Ray(p, d);

            // We already know that the ray hits both the objects
            // So we don't have to do any further intersection tests.

            // Hit the left object first
            if(p.z >= leftStart.z && p.z <= rightEnd.z) {
                // Make the intersection on the more visible object (left)
                left->prim->Intersect(_ray_, isect);

                // Update the normal for the intersection
                // Hits the left object first so the normal
                // will be set to the right object.
//                right->prim->Intersect(_ray_, &rIsx);
                isect->normalGeometric = rIsx.normalGeometric;

                return true;
            }

            // Hit the right object first
            if(p.z >= rightStart.z && p.z <= leftEnd.z) {
                // Make the intersection on the more visible object (right)
                right->prim->Intersect(_ray_, isect);

                // Update the normal for the intersection
                // Hits the right object first so the normal
                // will be set to the left object
//                left->prim->Intersect(_ray_, &lIsx);
                isect->normalGeometric = lIsx.normalGeometric;

                return true;
            }

            // Increment t for next step
            t += step;
        }
    }

    // Doesn't hit either of the primitives so reset the intersection
    *isect = Intersection();
    // Return false since the ray doesn't hit the primitives
    return false;
}


