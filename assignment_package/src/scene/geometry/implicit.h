#pragma once
#include "shape.h"

class Implicit : public Shape
{
public:
    virtual bool Intersect(const Ray &ray, Intersection *isect) const;
    virtual Point2f GetUVCoordinates(const Point3f &point) const;
    virtual void ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const;

    // Since ellipsoids should technically be a different
    // class of Shape than Sphere, you may assume that a Sphere
    // has been scaled uniformly when computing its surface area.
    virtual float Area() const;

    // Sample a point on the surface of the shape and return the PDF with
    // respect to area on the surface.
    virtual Intersection Sample(const Point2f &xi, Float *pdf) const;

    Bounds3f WorldBound() const;

    void create();
};