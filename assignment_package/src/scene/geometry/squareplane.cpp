#include "squareplane.h"

float SquarePlane::Area() const
{
    // Area is in world space

    Vector3f scale = this->transform.getScale();

    return scale.x * scale.y;
}

bool SquarePlane::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void SquarePlane::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    //TODO: Compute tangent and bitangent

    // Transforming a tangent vector from object space to world space
    // does not use the inverse transpose model matrix
    // It just uses the model matrix

    // tan is found by transforming i_hat to world space
    *tan = glm::normalize(transform.T3() * Normal3f(1,0,0));

    // bit is found by crossing the nor and tan in world space
    *bit = glm::normalize(glm::cross(*nor, *tan));
}


Point2f SquarePlane::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(point.x + 0.5f, point.y + 0.5f);
}

Intersection SquarePlane::Sample(const Point2f &xi, Float *pdf) const
{
    Point3f p(xi.x - 0.5f, xi.y - 0.5f, 0.f);

    Intersection intersection = Intersection();

    intersection.normalGeometric = glm::normalize(this->transform.invTransT() * Normal3f(0,0,1));

    intersection.point = glm::vec3(this->transform.T() * glm::vec4(p, 1.f));

    return intersection;
}

Bounds3f SquarePlane::WorldBound() const
{
    Point3f min(-0.5f, -0.5, 0.f);
    Point3f max(0.5f, 0.5f, 0.f);

    Bounds3f bounds(min, max);
    bounds = bounds.Apply(transform);

    return bounds;
}
