#include "specularbTdf.h"

Color3f SpecularBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}


/// Refract wo to get wi.
/// Check for total internal reflection --> return black.
/// This class only handles trasmission so don't compute color
/// that
Color3f SpecularBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO!
    Color3f color(0.f);

    // Figure out which n is incident and which is transmitted
    bool entering = CosTheta(wo) > 0;

    float etaI = entering ? etaA : etaB;
    float etaT = entering ? etaB : etaA;

    // Compute ray direction for specular transmission
    if(!Refract(wo, Faceforward(Normal3f(0,0,1), wo), etaI / etaT, wi)) {
        return color;
    }

    *pdf = 1.f;

    color = T * (Color3f(1.f) - fresnel->Evaluate(wi->z));

    // Account for non-symmetry with transmission to different medium

    color /= AbsCosTheta(*wi);

    return color;
}
