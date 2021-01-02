#include "bssrdf.h"

// Straight from PBRT
Float FresnelMoment1(Float eta) {
	Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta,
		eta5 = eta4 * eta;
	if (eta < 1)
		return 0.45966f - 1.73965f * eta + 3.37668f * eta2 - 3.904945 * eta3 +
		2.49277f * eta4 - 0.68441f * eta5;
	else
		return -4.61686f + 11.1136f * eta - 10.4646f * eta2 + 5.11455f * eta3 -
		1.27198f * eta4 + 0.12746f * eta5;
}

Float FresnelMoment2(Float eta) {
	Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta,
		eta5 = eta4 * eta;
	if (eta < 1) {
		return 0.27614f - 0.87350f * eta + 1.12077f * eta2 - 0.65095f * eta3 +
			0.07883f * eta4 + 0.04860f * eta5;
	}
	else {
		Float r_eta = 1 / eta, r_eta2 = r_eta * r_eta, r_eta3 = r_eta2 * r_eta;
		return -547.033f + 45.3087f * r_eta3 - 218.725f * r_eta2 +
			458.843f * r_eta + 404.557f * eta - 189.519f * eta2 +
			54.9327f * eta3 - 9.00603f * eta4 + 0.63942f * eta5;
	}
}

// Why are there some forward declaration issues in the header????
BSSRDF::BSSRDF(const Intersection& po, float eta) : po(po), eta(eta)
{
	if (po.point.x == 0)
		printf("SDLJ");
}

SeparableBSSRDF::SeparableBSSRDF(const Intersection& po, float eta) :
	BSSRDF(po, eta)
	, fresnel(1, eta)
	, ns(po.normalGeometric)
	, ts(po.tangent)
	, ss(po.bitangent)
{
}

Color3f SeparableBSSRDF::S(const Intersection& pi, const Vector3f& wi)
{
	return BLUE;
}

Color3f SeparableBSSRDF::Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf)
{
	auto S = Sample_Sp(scene, u1, u2, isect, pdf);
	if (!IsBlack(S))
	{
		// Add bxdf?
		return S;
	}
	else
	{
		return WHITE;
	}
}

Color3f SeparableBSSRDF::Sw(const Vector3f& w) const
{
	//float c = 1 - 2 * FresnelMoment1(1 / eta);
	//return (1 - FrDielectric(CosTheta(w), 1, eta)) / (c * Pi);
	return BLACK;
}

Color3f SeparableBSSRDF::Sp(const Intersection& pi) const
{
	return BLACK;
}

Color3f SeparableBSSRDF::Sample_Sp(const Scene& scene, float u1, const Point2f& u2, Intersection* isect,
	float* pdf) const
{
	return BLACK;
}

float SeparableBSSRDF::Pdf_Sp(const Intersection& isect) const
{
	return 0;
}

JensenBSSRDF::JensenBSSRDF(const Intersection& po, float eta, const Color3f sigmaA, const Color3f sigmaS) :
	SeparableBSSRDF(po, eta)
	, sigmaA(sigmaA)
	, sigmaS(sigmaS)
	, sigmaT(sigmaA + sigmaS)
{
	// todo rho
}

Color3f JensenBSSRDF::Sr(float d) const
{
	return BLACK;
}

float JensenBSSRDF::Sample_Sr(int ch, float u) const
{
	return 0;
}

float JensenBSSRDF::Pdf_Sr(int ch, float r) const
{
	return 0;
}
