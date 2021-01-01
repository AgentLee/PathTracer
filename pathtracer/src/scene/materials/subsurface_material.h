#pragma once
#include "material.h"

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
		
	}

	Color3f Kd, Kt, Kr;
	float eta;
	glm::vec3 sigmaA, sigmaS;
};
