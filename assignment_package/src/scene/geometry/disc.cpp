#include "disc.h"
#include "warpfunctions.h"

float Disc::Area() const
{
    // Area is in world space

    return Pi * (this->transform.getScale().x * this->transform.getScale().y);
}

bool Disc::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 1.f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void Disc::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    //TODO: Compute tangent and bitangent

//    glm::vec4 t = transform.T() * glm::vec4(1,0,0,0);
//    glm::vec4 b = transform.T() * glm::vec4(0,1,0,0);

    *tan = glm::normalize(transform.T3() * Normal3f(1,0,0));
    *bit = glm::normalize(glm::cross(*nor, *tan));
}


Point2f Disc::GetUVCoordinates(const Point3f &point) const
{
    return glm::vec2((point.x + 1)/2.f, (point.y + 1)/2.f);
}

Intersection Disc::Sample(const Point2f &xi, Float *pdf) const
{
    // Generate world space point on the surface of the shape
    // test with this radius first

    Point3f point(WarpFunctions::squareToDiskConcentric(xi).x /** 0.5f*/,
                  WarpFunctions::squareToDiskConcentric(xi).y /** 0.5f*/,
                  0.f
                  );

    Intersection intersection = Intersection();

    intersection.normalGeometric = glm::normalize(this->transform.invTransT() * Normal3f(0,0,1));

    intersection.point = glm::vec3(this->transform.T() * glm::vec4(point, 1.f));

    return intersection;
}

Bounds3f Disc::WorldBound() const
{
    Point3f min(-0.5f, -0.5f, 0.f);
    Point3f max(0.5f, 0.5f, 0.f);

    Bounds3f bounds(min, max);
    bounds = bounds.Apply(transform);

    return bounds;
}
