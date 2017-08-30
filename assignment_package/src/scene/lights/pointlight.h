#pragma once

#include "light.h"

class PointLight : public Light
{
public:
    PointLight(const Transform &t, const Color3f &Le) :
        Light(t),
        pLight(Point3f(t.T() * glm::vec4(0.f,0.f,0.f,1.f))),
        emittedLight(Le)
    {}

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const;

    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;

    const Point3f pLight;
    const Color3f emittedLight;
};
