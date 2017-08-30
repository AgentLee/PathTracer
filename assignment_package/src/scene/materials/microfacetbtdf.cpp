#include "microfacetbtdf.h"

Color3f MicroFacetBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    // TODO

    if(SameHemisphere(wo, wi)) {
        return Color3f(0.f);
    }

    float cosThetaO = CosTheta(wo);
    float cosThetaI = CosTheta(wi);

    if(cosThetaI == 0 || cosThetaO == 0) {
        return Color3f(0.f);
    }

    float eta = CosTheta(wo) > 0.f ? (etaB / etaA) : (etaA / etaB);

    Vector3f wh = glm::normalize(wo + wi *eta);

    if(wh.z < 0.f) {
        wh = -wh;
    }

    Color3f F = fresnel.Evaluate(glm::dot(wo, wh));

    float sqrtDenom = glm::dot(wo, wh) + eta * glm::dot(wi, wh);
//    float factor =

    return (Color3f(1.f) - F) * T *
            std::abs(distribution->D(wh) * distribution->G(wo, wi)
                     * eta * eta* AbsDot(wi, wh) * AbsDot(wo, wh) /
                     (cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));
}

bool refract(const Vector3f &wi, const Normal3f &n, float eta, Vector3f *wt)
{
    // Compute cosTheta using Snell's law
    float cosThetaI = glm::dot(n, wi);
    float Sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
    float Sin2ThetaT = eta * eta * Sin2ThetaI;

    // Handle total internal reflection
    if(Sin2ThetaI >= 1.f) {
        return false;
    }

    float cosThetaT = std::sqrt(1 - Sin2ThetaT);

    *wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);

    return true;
}

Color3f MicroFacetBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    // TODO

    if(wo.z == 0) {
        return Color3f(0.f);
    }

    Vector3f wh = distribution->Sample_wh(wo, xi);

    float eta = CosTheta(wo) > 0 ? (etaA, etaB) : (etaB / etaA);

    if(!refract(wo, (Normal3f)wh, eta, wi)) {
        return Color3f(0.f);
    }

    *pdf = Pdf(wo, *wi);

    return f(wo, *wi);
}

float MicroFacetBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    // TODO

    if(SameHemisphere(wo, wi)) {
        return 0.f;
    }

    float eta = CosTheta(wo) > 0.f ? (etaB / etaA) : (etaA / etaB);

    Vector3f wh = glm::normalize(wo + wi * eta);

    float sqrtDenom = glm::dot(wo, wh) + eta * glm::dot(wi, wh);

    float dwh_dwi = std::abs((eta * eta * glm::dot(wi, wh)) /
                             (sqrtDenom* sqrtDenom));

    return distribution->Pdf(wo, wh) * dwh_dwi;
}
