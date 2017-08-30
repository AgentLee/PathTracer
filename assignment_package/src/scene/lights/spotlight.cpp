#include "spotlight.h"

Color3f Spotlight::L(const Intersection &isect, const Vector3f &w) const
{
    return Color3f(0.f);
}

Color3f Spotlight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.f;

    return emittedLight * falloff(-*wi) / glm::distance2(pLight, ref.point);
}

float Spotlight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.f;
}

float Spotlight::falloff(const Vector3f &w) const
{
    Vector3f wl = glm::normalize(Vector3f(transform.invT() * glm::vec4(w, 1.f)));
    float cosTheta = wl.z;

    if(cosTheta < cosTotalWidth)
        return 0.f;

    if(cosTheta > cosFalloffStart)
        return 1.f;

    float delta = (cosTheta - cosTotalWidth) /
            (cosFalloffStart - cosTotalWidth);

    return (delta * delta) * (delta * delta);
}
