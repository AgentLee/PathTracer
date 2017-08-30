#include "orennayar.h"
#include <warpfunctions.h>

// Set to true for cosine weighted
// Set to false for uniform
#define COSINE true

Color3f OrenNayarBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO

    float sinThetaI = std::sqrt(1.f - pow(CosTheta(wi), 2));
    float sinThetaO = std::sqrt(1.f - pow(CosTheta(wo), 2));

    // find cos
    float maxCos = 0.f;
    if(sinThetaI > 0.0001f && sinThetaO > 0.0001f) {
        float cosPhiI = std::acos(wi.x);
        float cosPhiO = std::acos(wo.x);
        float sinPhiI = std::sqrt(1.f - pow(cosPhiI, 2));
        float sinPhiO = std::sqrt(1.f - pow(cosPhiO, 2));

        float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;

        maxCos = std::max(0.f, dCos);
    }

    // find sin and tan
    float sinAlpha;
    float tanBeta;

    if(AbsCosTheta(wi) > AbsCosTheta(wo)) {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / AbsCosTheta(wi);
    } else {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / AbsCosTheta(wo);
    }

    return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
}

Color3f OrenNayarBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO

    if(COSINE) {
        *wi = WarpFunctions::squareToHemisphereCosine(u);
    } else {
        *wi = WarpFunctions::squareToHemisphereUniform(u);
    }

    if(wo.z < 0.f) {
        wi->z *= -1.f;
    }

    *pdf = Pdf(wo, *wi);

    return f(wo, *wi);
}

float OrenNayarBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO

    // Following Malley's Method for sampling pdf
    // cos(theta) = wi.z
    return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}
