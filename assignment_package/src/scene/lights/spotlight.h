#pragma once

#include "light.h"

class Spotlight : public Light
{
public:
    Spotlight(const Transform &t, const Color3f &Le, float totalWidth, float falloffStart) :
        Light(t), pLight(Point3f(t.T() * glm::vec4(0.f,0.f,0.f,1.f))), emittedLight(Le),
        cosTotalWidth(std::cos(glm::radians(totalWidth))),
        cosFalloffStart(std::cos(glm::radians(falloffStart))),
        transform(t)
    {}

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const;

    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;

    float falloff(const Vector3f &w) const;

    const Transform transform;
    const Point3f pLight;
    const Color3f emittedLight;
    const float cosTotalWidth, cosFalloffStart;
};
