#include "lambertbrdf.h"
#include <warpfunctions.h>

// Set to true for cosine weighted
// Set to false for uniform
#define COSINE true

Color3f LambertBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO

    return R * InvPi;
}

Color3f LambertBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
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

float LambertBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO

    // Following Malley's Method for sampling pdf
    // cos(theta) = wi.z
    return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}
