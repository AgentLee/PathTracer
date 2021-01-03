#include "bssrdf.h"
#include "scene/scene.h"

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
		isect->bsdf = std::make_shared<BSDF>(*isect, eta);
		// Need to add a dummy BSDF
		isect->bsdf->Add(new SeparableBSSRDFAdapter(this));
		isect->wo = isect->normalGeometric;
	}

	return S;
}

Color3f SeparableBSSRDF::Sw(const Vector3f& w) const
{
	float c = 1 - 2 * FresnelMoment1(1 / eta);
	return (glm::vec3(1) - fresnel.Evaluate(CosTheta(w))) / (c * Pi);
}

Color3f SeparableBSSRDF::Sp(const Intersection& pi) const
{
	return Sr(glm::distance(pi.point, po.point));
}

// Find exit point to sample after bouncing around inside the material.
// Jensen paper doesn't go over how to importance sample.
// King paper describes the disk sampling method which is also used in PBRT.
Color3f SeparableBSSRDF::Sample_Sp(const Scene& scene, float u1, const Point2f& u2, Intersection* isect,
	float* pdf) const
{
	// Pick an axis to sample from. It's biased towards the normal.
	Vector3f vx, vy, vz;
	if(u1 < 0.5f)
	{
		vx = ss;
		vy = ts;
		vz = ns;
		u1 *= 2.0f;
	}
	else if(u1 < 0.75f)
	{
		vx = ts;
		vy = ns;
		vz = ss;
		u1 = (u1 - 0.5f) * 4.0f;
	}
	else
	{
		vx = ns;
		vy = ss;
		vz = ts;
		u1 = (u1 - 0.75f) * 4.0f;
	}

	// Pick one of the color channels to sample from.
	int ch = glm::clamp((int)(u1 * NUM_CHANNELS), 0, NUM_CHANNELS - 1);
	u1 = u1 * NUM_CHANNELS - ch;

	// Sample BSSRDF profile in polar coordinates.
	// Sample_Sr is virtual so it'll go to the derived implementation of it.
	float r = Sample_Sr(ch, u1);
	if(r < 0)
		return BLACK;
	float phi = 2.0f * Pi * u2.y;

	// I can't get past this check. :/
	
	// Compute BSSRDF profile bounds and intersection height.
	float rMax = Sample_Sr(ch, 0.999f);
	rMax = 16.0f;
	if(r >= rMax)
		return BLACK;
	float l = 2.0f * sqrt(rMax * rMax - r * r);

	// Compute BSSRDF sampling ray segment.
	Intersection base;
	base.point= po.point + r * (vx * cos(phi) + vy * sin(phi)) - l * vz * 0.5f;
	auto pTarget = base.point + l * vz;

	struct IntersectionChain
	{
		Intersection si;
		IntersectionChain* next = nullptr;
	};
	
	IntersectionChain* chain = new IntersectionChain();
	IntersectionChain* ptr = chain;
	int numIsx = 0;
	
	// Shoot the BSSRDF ray into the scene.
	// Accumulate intersections along the ray.
	while(true)
	{
		Ray r = base.SpawnRay(pTarget);
		auto intersects = scene.Intersect(r, &ptr->si);
		if(!intersects || r.direction == Vector3f(0.0f))
		{
			break;
		}

		base = ptr->si;

		if(po.objectHit->material == ptr->si.objectHit->material)
		{
			auto* next = new IntersectionChain();
			ptr->next = next;
			ptr = next;
			numIsx++;
		}
	}

	// Choose one of the intersections.
	{
		if (numIsx == 0)
			return BLACK;

		int selected = glm::clamp((int)(u1* numIsx), 0, numIsx - 1);
		while (selected-- > 0)
		{
			chain = chain->next;
		}

		*isect = std::move(chain->si);

		// todo delete linked list
	}
	
	// Compute pdf.
	*pdf = Pdf_Sp(*isect) / numIsx;
	*pdf = 0.5;

	// Return spatial profile. 
	return Sp(*isect);
}

// Returns the probability of picking exit point.
float SeparableBSSRDF::Pdf_Sp(const Intersection& pi) const
{
	using namespace glm;
	
	auto d = pi.point - po.point;
	auto dLocal = Vector3f(dot(ss, d), dot(ts, d), dot(ns, d));
	auto nLocal = Vector3f(dot(ss, pi.normalGeometric), dot(ts, pi.normalGeometric), dot(ns, pi.normalGeometric));

	// Profile radius under projection along each axis.
	float rProj[3] = {	std::sqrt(dLocal.y * dLocal.y + dLocal.z * dLocal.z),
						std::sqrt(dLocal.z * dLocal.z + dLocal.x * dLocal.x),
						std::sqrt(dLocal.x * dLocal.x + dLocal.y * dLocal.y) };

	float pdf = 0;
	float axisProb[3] = { .25f, .25f, .5f };
	float chProb = 1.0f / NUM_CHANNELS;
	for(int axis = 0; axis < 3; ++axis)
	{
		for(int ch = 0; ch < NUM_CHANNELS; ++ch)
		{
			pdf += Pdf_Sr(ch, rProj[axis]) * std::abs(nLocal[axis]) * chProb * axisProb[axis];
		}
	}
	
	return pdf;
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
	// TODO
	using namespace glm;

	auto alphaPrime = sigmaS / sigmaT;
	auto D = 1.0f / (3.0f * sigmaT);
	auto sigmaTr = sqrt(3.0f * sigmaA * sigmaT);

	auto Fdr = [this]()
	{
		return (-1.440f / pow(eta, 2.0f)) + (0.710f / eta) + 0.668f + (0.0636f * eta);
	};

	auto A = (1 + Fdr()) / (1 - Fdr());
	
	auto zr = 1.0f / sigmaT;
	auto zv = zr + (4 * A * D);
	auto dr = sqrt(zr * zr + d * d);
	auto dv = sqrt(zv * zv + d * d);

	auto fadeTerm = sigmaTr * dr + 1.0f;	// rename?
	auto leftFrac = exp(-sigmaTr * dr) / (sigmaT * dr * dr * dr);
	auto lhs = fadeTerm * leftFrac;
	auto rightFrac = exp(-sigmaTr * dv) / (sigmaTr * dv * dv * dv);
	auto rhs = zv * fadeTerm * rightFrac;

	auto Rd = (alphaPrime / FourPi) * (lhs + rhs);

	float scale = 1;
	return scale * Rd;
}

float JensenBSSRDF::Sample_Sr(int ch, float u) const
{
	// TODO
	// Err not really following PBRT for this.
	// 1 / sigmaT is directly from Jensen.
	//auto st = sigmaT[ch];
	//return st >  0 ? (1 / st) : -1;

	//https://github.com/JiayinCao/SORT/blob/379a900b8d8f9832ed10518b2807784bd6405d08/src/scatteringevent/bsdf/disney.cpp
	float ret = 0;
	auto st = sigmaT[ch];
	if(u < 0.25f)
	{
		ret = -st * log(4.0f * u);
	}
	else
	{
		ret = -3.0f * st * log((u - .25f) * 1.3333f);
	}

	if(ret > 16 * st)
	{
		return -1;
	}
	else
	{
		return ret;
	}
}

float JensenBSSRDF::Pdf_Sr(int ch, float r) const
{
	return 0;
}

SeparableBSSRDFAdapter::SeparableBSSRDFAdapter(const SeparableBSSRDF* bssrdf) :
	BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
	bssrdf(bssrdf)
{}

Color3f SeparableBSSRDFAdapter::f(const Vector3f& woW, const Vector3f& wiW) const
{
	auto f = bssrdf->Sw(wiW);
	// account for transport mode?
	return f;
}
