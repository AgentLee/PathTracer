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

//Color3f SeparableBSSRDF::S(const Intersection& pi, const Vector3f& wi)
//{
//	auto cosDotI = glm::dot(po.normalGeometric, po.wo);
//	auto Ft = 1 - fresnel.EvaulateF(cosDotI);
//	return Ft * Sp(pi) * Sw(wi);
//}
//
//Color3f SeparableBSSRDF::Sw(const Vector3f& w) const
//{
//	float c = 1 - 2 * FresnelMoment1(1 / eta);
//	auto fr = fresnel.Evaluate(CosTheta(w));
//	return (1.0f - fr) / (c * Pi);
//}
//
//Color3f SeparableBSSRDF::Sp(const Intersection& pi) const
//{
//	return Sr(glm::distance(po.point, pi.point));
//}
//
//Color3f SeparableBSSRDF::Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect,
//	float* pdf) const
//{
//	return Color3f(0.0f);
//}
//
//Color3f SeparableBSSRDF::Sample_Sp(const Scene& scene, float u1, const Point2f& u2, Intersection* isect,
//	float* pdf) const
//{
//	// Choose projection axis to sample.
//	Vector3f x, y, z;
//	if(u1 < 0.5f)
//	{
//		// Sample with respect to ns (normal) at Z.
//		// This is the best sample option.
//		x = ss;
//		y = ts;
//		z = Vector3f(ns);
//
//		u1 *= 2.0f;
//	}
//	else if(u1 < 0.75f)
//	{
//		// Sample with respect to ss (bitangent) at Z.
//		x = ts;
//		y = Vector3f(ns);
//		z = ss;
//
//		u1 = (u1 - 0.5f)* 4.0f;
//	}
//	else
//	{
//		// Sample with respect to ts (tangent) at Z.
//		x = ss;
//		y = Vector3f(ns);
//		z = ts;
//
//		u1 = (u1 - 0.75f) * 4.0f;
//	}
//
//	// Choose spectral (color) channel for sampling.
//	// We have 3 color channels - https://github.com/gao-duan/EDXRay/blob/master/EDXRay/Core/BSSRDF.cpp
//	auto ch = glm::clamp((u1 * NUM_CHANNELS), 0.0f, 1.0f);
//	u1 = u1 * 3.0f - ch;
//
//	// Sample BSSRDF profile in polar coordinates.
//	float r = Sample_Sr(ch, u2.x);
//	if(r < 0.0f)
//		return BLACK;
//	float phi = 2.0f * Pi * u2.x;
//
//	// Compute BSSRDF profile bounds and intersection height.
//	float rMax = Sample_Sr(ch, 0.999f);
//	if(r > rMax)
//		return BLACK;
//	float l = 2.0f * sqrt(rMax * rMax - r * r);
//
//	// Compute BSSRDF ray sampling segment.
//	Intersection base;
//	base.point = po.point + r * (x * cos(phi) + y * sin(phi)) - l * z  * 0.5f;
//	Point3f target = base.point + l * z;
//
//	// Shoot the BSSRDF ray into the scene.
//	struct IntersectionChain
//	{
//		Intersection isect;
//		IntersectionChain* next = nullptr;
//	};
//
//	// A little skeptical if this will work. Might have to allocate it to an actual size.
//	IntersectionChain* chain = new IntersectionChain();
//	auto ptr = chain;
//	int numIsx = 0;
//
//	// As long as we keep hitting something and it's the same material, we add it to the chain.
//	auto ray = base.SpawnRay(target);
//	while(scene.Intersect(ray, &base))
//	{
//		base = ptr->isect;
//		if(ptr->isect.material == material)
//		{
//			auto* next = new IntersectionChain();
//			ptr->next = next;
//			ptr = next;
//			numIsx++;
//		}
//	}
//
//	// Randomly choose one of the intersections to sample.
//	if(numIsx == 0)
//	{
//		return BLACK;		
//	}
//
//	int selected = glm::clamp((int)(u1 * numIsx), 0, numIsx - 1);
//	while(selected-- > 0)
//	{
//		chain = chain->next;
//	}
//	*isect = chain->isect;
//
//	// Compute sample PDF and return Sp
//	*pdf = Pdf_Sp(*isect) / numIsx;
//	return Sp(*isect);
//}
//
//float SeparableBSSRDF::Pdf_Sp(const Intersection& isect) const
//{
//	auto d = isect.point - po.point;
//	auto dLocal = Vector3f(glm::dot(ss, d), glm::dot(ts, d), glm::dot(ns, d));
//	auto nLocal = Normal3f(glm::dot(ss, isect.normalGeometric), glm::dot(ts, isect.normalGeometric), glm::dot(ns, isect.normalGeometric));
//
//	// Profile radius under each axis
//	float rProj[3] = {	sqrt(dLocal.y * dLocal.y + dLocal.z * dLocal.z),
//						sqrt(dLocal.z * dLocal.z + dLocal.x * dLocal.x),
//						sqrt(dLocal.x * dLocal.x + dLocal.y * dLocal.y)
//	};
//
//	// Return probability
//	float pdf = 0.0f;
//	float axisProb[3] = { 0.25f, 0.25f, 0.5f };
//	float chProb = 1.0f / NUM_CHANNELS;
//	for(int axis = 0; axis < 3; ++axis)
//	{
//		for(int ch = 0; ch < NUM_CHANNELS; ++ch)
//		{
//			pdf += Pdf_Sr(ch, rProj[axis]) * abs(nLocal[axis]) * chProb * axisProb[axis];
//		}
//	}
//	
//	return pdf;
//}
