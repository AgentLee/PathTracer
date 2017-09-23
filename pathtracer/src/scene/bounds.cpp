#include "bounds.h"

// Computes the intersection given the ray
// Only computes the t value and returns true/false on intersection
// -t values are valid iff the ray's origin lies within the box
bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    //TODO
    float t_n = -1000000;
    float t_f = 1000000;

    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r.direction[i] == 0){
            if(r.origin[i] < this->min[i] || r.origin[i] > this->max[i]){
                return false;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (this->min[i] - r.origin[i])/r.direction[i];
        float t1 = (this->max[i] - r.origin[i])/r.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    if(t_n < t_f)
    {
        float t_tmp = t_n > 0 ? t_n : t_f;

        bool inBox = r.origin.x > this->min.x && r.origin.x < max.x &&
                     r.origin.y > this->min.y && r.origin.y < max.y &&
                     r.origin.z > this->min.z && r.origin.z < max.z;

        if(t_tmp < 0.f) {
            if(!inBox) {
                return false;
            }
        }

        *t = t_tmp;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return false;
    }
}

// Transform the corners of the bounding box based on the
// input matrix. From this compute a new bounding box that
// encompasses these corners. The invoker is a Bounds3f and
// return a new Bounds3f with the transformations applied.
Bounds3f Bounds3f::Apply(const Transform &tr)
{
    //TODO

    Point3f c1 = Point3f(tr.T() * glm::vec4(min.x, max.y, min.z, 1.f));
    Point3f c2 = Point3f(tr.T() * glm::vec4(min.x, min.y, min.z, 1.f));
    Point3f c3 = Point3f(tr.T() * glm::vec4(max.x, max.y, min.z, 1.f));
    Point3f c4 = Point3f(tr.T() * glm::vec4(max.x, min.y, min.z, 1.f));
    Point3f c5 = Point3f(tr.T() * glm::vec4(min.x, max.y, max.z, 1.f));
    Point3f c6 = Point3f(tr.T() * glm::vec4(min.x, min.y, max.z, 1.f));
    Point3f c7 = Point3f(tr.T() * glm::vec4(max.x, max.y, max.z, 1.f));
    Point3f c8 = Point3f(tr.T() * glm::vec4(max.x, min.y, max.z, 1.f));

    Bounds3f bounds = Union(Union(Union(Union(Union(Union(Union(Bounds3f(c1), c2), c3), c4), c5), c6), c7), c8);

    this->min = bounds.min;
    this->max = bounds.max;

    return bounds;

//    Bounds3f transformedBounds(Point3f(tr.T() * glm::vec4(min.x, min.y, min.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(max.x, min.y, min.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(min.x, max.y, min.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(min.x, min.y, max.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(min.x, max.y, max.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(max.x, max.y, min.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(max.x, min.y, max.z, 1.f)));
//    transformedBounds = Union(transformedBounds, Point3f(tr.T() * glm::vec4(max.x, max.y, max.z, 1.f)));

//    this->min = transformedBounds.min;
//    this->max = transformedBounds.max;

//    return transformedBounds;
}

// Returns the surface area of the invoking Bounds3f.
// Use this when applying the surface area heuristic
// to split the BVH.
float Bounds3f::SurfaceArea() const
{
    //TODO

    Vector3f d = Diagonal();

    return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

// Finds the Bounds3f(min, max)
Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2)
{
    return Bounds3f(Point3f(std::min(b1.min.x, b2.min.x),
                            std::min(b1.min.y, b2.min.y),
                            std::min(b1.min.z, b2.min.z)),
                    Point3f(std::max(b1.max.x, b2.max.x),
                            std::max(b1.max.y, b2.max.y),
                            std::max(b1.max.z, b2.max.z)));
}

Bounds3f Union(const Bounds3f& b1, const Point3f& p)
{
    return Bounds3f(Point3f(std::min(b1.min.x, p.x),
                            std::min(b1.min.y, p.y),
                            std::min(b1.min.z, p.z)),
                    Point3f(std::max(b1.max.x, p.x),
                            std::max(b1.max.y, p.y),
                            std::max(b1.max.z, p.z)));
}

Bounds3f Union(const Bounds3f& b1, const glm::vec4& p)
{
    return Union(b1, Point3f(p));
}
