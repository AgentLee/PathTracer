#include "environmentlight.h"

Color3f EnvironmentLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf) const
{
    Vector3f dir = -glm::normalize(*wi);

    float u = 0.5f + (std::atan2(dir.z / Pi, dir.x / Pi)) * Inv2Pi;
    float v = 0.5f + dir.y * 0.5f;

    Point2f uv(u, v);

    Color3f col = Material::GetImageColor(uv, this->environmentMap.get());

    return col * intensity;
}

float EnvironmentLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 1.0f;
}

Color3f EnvironmentLight::L(const Intersection &isect, const Vector3f &w) const
{
    Vector3f dir = glm::normalize(w);

    float u = 0.5f + (std::atan2(dir.z / Pi, dir.x / Pi)) * Inv2Pi;
    float v = 0.5f + dir.y * 0.5f;

    Point2f uv(u, v);

    Color3f col = Material::GetImageColor(uv, this->environmentMap.get());

    return col * intensity;
}
