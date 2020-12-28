#include "bsdf.h"
#include <warpfunctions.h>

// Set to true for cosine weighted
// Set to false for uniform
#define COSINE true

BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
//TODO: Properly set worldToTangent and tangentToWorld
    : worldToTangent(Matrix3x3(0.f)),
      tangentToWorld(Matrix3x3(0.f)),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{
    UpdateTangentSpaceMatrices(isect.normalGeometric, isect.tangent, isect.bitangent);
}

BSDF::~BSDF()
{
    for(int i = 0; i < numBxDFs; i++) {
        delete bxdfs[i];
    }
}

void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    //TODO: Update worldToTangent and tangentToWorld based on the normal, tangent, and bitangent

    tangentToWorld = Matrix3x3(t, b, n);
    worldToTangent = glm::transpose(tangentToWorld);
}


//
Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    //TODO

    Color3f color(0.f);

    // Convert w's to tangent space
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT = worldToTangent * wiW;

    // Determines if we should use BRDF or BTDF
    // if wi and wo are in the same hemisphere wrt the geometric normal --> reflection
    // else if wi and wo are in different hemispheres --> transmission
    //
    // In the scattering equation, however,
    // use the surface normal rather than the geometric normal.
    bool reflect = glm::dot(wiW, normal) * glm::dot(woW, normal) > 0.f;

    for(int i = 0; i < numBxDFs; ++i) {
        // if reflective or not transmissive
        if( bxdfs[i]->MatchesFlags(flags) &&
            ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
             (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
        {
            color += bxdfs[i]->f(woT, wiT);
        }
    }

    return color;
}

// Use the input random number _xi_ to select
// one of our BxDFs that matches the _type_ flags.

// After selecting our random BxDF, rewrite the first uniform
// random number contained within _xi_ to another number within
// [0, 1) so that we don't bias the _wi_ sample generated from
// BxDF::Sample_f.

// Convert woW and wiW into tangent space and pass them to
// the chosen BxDF's Sample_f (along with pdf).
// Store the color returned by BxDF::Sample_f and convert
// the _wi_ obtained from this function back into world space.

// Iterate over all BxDFs that we DID NOT select above (so, all
// but the one sampled BxDF) and add their PDFs to the PDF we obtained
// from BxDF::Sample_f, then average them all together.

// Finally, iterate over all BxDFs and sum together the results of their
// f() for the chosen wo and wi, then return that sum.

Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType) const
{
    //TODO
    Color3f color(0.f);

    // We use a random point because it'll eventually converge to some color
    // which is the point of Monte Carlo. The more samples that you take, the
    // more of the integral gets filled in.
    //
    // So if that if a type is specular, the things that are specular will be
    // rendered and if the type is specular and reflective, things that are
    // both will get rendered.

///         Choose BxDF     ///
    // How many components match the BxDF passed in
    int matchingComps = BxDFsMatchingFlags(type);
    if(matchingComps == 0) {
        *pdf = 0;

        return color;
    }

    // The first dimension of the sample xi is used to select
    // one of the components with equal probability.
    //
    // We did some bitshifting in the beginning
    // which is why we find the minimum one
    int comp = std::min((int)std::floor(xi[0] * matchingComps), matchingComps - 1);

///         Get BxDF pointer for component       ///
    BxDF *bxdf = nullptr;   // tbd chosen bxdf

    // Need to loop through the set of BxDFs to find
    // the exact BxDF that matches the flags.
    int count = comp;
    for(int i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i]->MatchesFlags(type) && count-- == 0) {
            bxdf = bxdfs[i];

            break;
        }
    }

///          Remap BxDF sample       ///
    // Still not quite sure why PBRT mentions [0, 0.5]
    // --> hypothetical example
    Point2f remap(xi.x * matchingComps - comp, xi.y);

///         Sample chosen BxDF      ///
    // The BxDF that was chosen can now be sampled.
    // Convert woW and wiW to tangent space
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT(0.f);// = worldToTangent * woW;

    // Find PDF for the sampling direction wi.
    // Average all the PDFs that could have been used.
    *pdf = 0.f;

    // If there's a material on this point that has been set
    // then we change the type to that because of the recursiveness.
    // I don't know how else to explain this or if it's right.
    // :p
    if(sampledType) {
        *sampledType = bxdf->type;
    }

    color = bxdf->Sample_f(woT, &wiT, remap, pdf, sampledType);

    if(*pdf == 0) {
        return Color3f(0.f);
    }

    *wiW = tangentToWorld * wiT;

///         Compute PDF with matching BxDFs         ///
    if(!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1) {
        for(int i = 0; i < numBxDFs; ++i) {
            if(bxdfs[i] != bxdf && bxdfs[i]->MatchesFlags(type)) {
                *pdf += bxdfs[i]->Pdf(woT, wiT);
            }
        }
    }

    if(matchingComps > 1) {
        *pdf /= matchingComps;
    }

///         Compute BSDF value for sample direction         ///
    if(!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1) {
        bool reflect = glm::dot(*wiW, normal) * glm::dot(woW, normal) > 0;

        color = Color3f(0.f);

        for(int i = 0; i < numBxDFs; ++i) {
            if(bxdfs[i]->MatchesFlags(type) &&
               ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION)))) {
                color += bxdfs[i]->f(woT, wiT);
            }
        }
    }

    return color;
}


float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
	if(numBxDFs == 0)
		return 0.0f;
	
    float total = 0.f;
    int count = 0;
    for(int i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i]->MatchesFlags(flags)) {
            total += bxdfs[i]->Pdf(woW, wiW);
            count++;
        }
    }

    return total / (float)count;
}

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    if(COSINE) {
        *wi = WarpFunctions::squareToHemisphereCosine(xi);
    } else {
        *wi = WarpFunctions::squareToHemisphereUniform(xi);
    }

    if(wo.z < 0.f) {
        wi->z *= -1.f;
    }

    *pdf = Pdf(wo, *wi);

    return f(wo, *wi);
}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return SameHemisphere(wo, wi) ? Inv2Pi : 0;
}
