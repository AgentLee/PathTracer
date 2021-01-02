#pragma once
#include <globals.h>

enum BxDFType {
	BSDF_REFLECTION = 1 << 0,   // This BxDF handles rays that are reflected off surfaces
	BSDF_TRANSMISSION = 1 << 1, // This BxDF handles rays that are transmitted through surfaces
	BSDF_DIFFUSE = 1 << 2,      // This BxDF represents diffuse energy scattering, which is uniformly random
	BSDF_GLOSSY = 1 << 3,       // This BxDF represents glossy energy scattering, which is biased toward certain directions
	BSDF_SPECULAR = 1 << 4,     // This BxDF handles specular energy scattering, which has no element of randomness
	BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION
};

class BxDF
{
public:
	BxDF(BxDFType type) : type(type) {}
	virtual ~BxDF() {}

	virtual Color3f f(const Vector3f &woW, const Vector3f &wiW) const = 0;

	// Generates a uniformly random sample on the hemisphere
	// and evaluates f() on it.
	// Specific BxDF subclasses can (and should) override this.
	virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
		const Point2f &xi, Float *pdf,
		BxDFType *sampledType = nullptr) const;

	// Evaluates the PDF for uniformly sampling a hemisphere.
	// Specific BxDF subclasses can (and should) override this.
	virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

	//Bitwise AND. This is not a reference.
	bool MatchesFlags(BxDFType t) const { return (type & t) == type; }

	const BxDFType type;
};