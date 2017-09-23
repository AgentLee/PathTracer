#pragma once
#include <globals.h>

class Ray
{
public:
    Ray(const Point3f &o, const Vector3f &d);
    Ray(const glm::vec4 &o, const glm::vec4 &d);
    Ray(const Ray &r);

    //Return a copy of this ray that has been transformed
    //by the input transformation matrix.
    Ray GetTransformedCopy(const Matrix4x4 &T) const;

    Point3f origin;
    Vector3f direction;

//    float tMax = std::numeric_limits<float>::infinity();
    float tMax = 100000.f;
};
