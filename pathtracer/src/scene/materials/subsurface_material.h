#pragma once
#include "bssrdf.h"
#include "material.h"
#include "specularbrdf.h"
#include "specularbtdf.h"

class SubsurfaceMaterial : public Material
{
public:
	SubsurfaceMaterial(	const Color3f& Kd,
						const Color3f& Kt,
						const Color3f& Kr,
						const float eta,
						const glm::vec3 sigmaA,
						const glm::vec3 sigmaS) :
		Kd(Kd), Kt(Kt), Kr(Kr), eta(eta), sigmaA(sigmaA), sigmaS(sigmaS)
	{}

	void ProduceBSDF(Intersection *isect) const
	{
		// PBRT created TabulatedBSSRDF for this.
		// It's pretty complicated so let's try to avoid it
		// and try to implement only Jensen's paper first.
		isect->bssrdf = std::make_shared<JensenBSSRDF>(*isect, eta, sigmaA, sigmaS);
		
		// Also need to make a BSDF for the reflective part
		isect->bsdf = std::make_shared<BSDF>(*isect, eta);
		
		// TODO
		// texture map
		// normal map

		// Add BxDFs - not sure which we need.
		isect->bsdf->Add(new SpecularBRDF(Kr, new FresnelDielectric(1.f, eta)));
		isect->bsdf->Add(new SpecularBTDF(Kt, 1.f, eta, new FresnelDielectric(1.f, eta)));
	}

	Color3f Kd, Kt, Kr;
	float eta;
	glm::vec3 sigmaA, sigmaS;
};
