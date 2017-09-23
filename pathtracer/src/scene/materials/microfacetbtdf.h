#pragma once
#include "bsdf.h"
#include "fresnel.h"
#include "microfacet.h"

class MicroFacetBTDF : public BxDF
{
public:
    MicroFacetBTDF(const Color3f &T,
                   MicrofacetDistribution *distribution,
                   float etaA,
                   float etaB,
                   int mode)
        : BxDF(BxDFType(BSDF_TRANSMISSION || BSDF_GLOSSY)),
          T(T),
          distribution(distribution),
          etaA(etaA),
          etaB(etaB),
          fresnel(etaA, etaB),
          mode(mode)
    {}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &xi, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

private:
    const Color3f T;
    const MicrofacetDistribution *distribution;
    const float etaA, etaB;
    const FresnelDielectric fresnel;
    const int mode;         // 0 for Radiance
                            // 1 for Importance
};
