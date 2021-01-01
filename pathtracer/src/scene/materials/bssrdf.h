#pragma once
#include <globals.h>
#include <raytracing/intersection.h>

#include "fresnel.h"
//#include "scene/scene.h"

class Intersection;
class BxDF;
class Scene;

// 3 color channels
#define NUM_CHANNELS 3

// Specifies where an intersection point came from (camera or light).
// Not sure how important this will be in the actual implementation.
enum TransportMode
{
	Radiance,		// Light
	Importance,		// Camera
};

class BSSRDF
{
public:
	BSSRDF(const Intersection& po, float eta = 1) : po(po), eta(eta) {}
	~BSSRDF() = default;

	Color3f S(const Intersection& pi, const Vector3f& wi) {return BLACK;}
	Color3f Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) { return BLACK; }
	
protected:
	const Intersection& po;
	float eta;
};

class SeparableBSSRDF : public BSSRDF
{
public:
	SeparableBSSRDF(const Intersection& po, float eta/*, const Material* material*/) : 
		BSSRDF(po, eta), fresnel(1, eta)/*, material(material) */
	{}

	~SeparableBSSRDF() = default;
	
	Color3f S(const Intersection& pi, const Vector3f& wi)
	{
		return BLACK;
	}
	Color3f Sw(const Vector3f& w) const
	{
		return BLACK;
	}
	Color3f Sp(const Intersection& pi) const
	{
		return BLACK;
	};

	Color3f Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) const
	{
		return BLACK;
	}
	Color3f Sample_Sp(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) const
	{
		return BLACK;
	}
	float Pdf_Sp(const Intersection& isect) const
	{
		return 0;
	}

	// Scattering profile gets supplied by a table
	virtual Color3f Sr(float d) const = 0;
	// Return negative for failed sample (no scattering from provided channel)
	virtual float Sample_Sr(int ch, float u) const = 0;
	virtual float Pdf_Sr(int ch, float r) const = 0;
	
private:
	//const Normal3f ns;	// normal
	//const Vector3f ss;	// bitangent
	//const Vector3f ts;	// tangent
	//const Material* material;
	const FresnelDielectric fresnel;
};

class JensenBSSRDF : public SeparableBSSRDF
{
public:
	JensenBSSRDF(const Intersection& po, /*Material* material,*/ float eta, const Color3f sigmaA, const Color3f sigmaS) :
		SeparableBSSRDF(po, eta)
	{
		sigmaT = sigmaA + sigmaS;

		// todo rho
	}
	
	~JensenBSSRDF() = default;
	
	Color3f Sr(float d) const override
	{
		return BLACK;
	}
	float Sample_Sr(int ch, float u) const override
	{
		return 0;
	}
	float Pdf_Sr(int ch, float r) const override
	{
		return 0;
	}

private:
	Color3f sigmaT;
	Color3f rho;
};



//class SeparableBSSRDF : public BSSRDF
//{
//public:
//	// using the tangent and bitangent might not work...
//	SeparableBSSRDF(const Intersection& po, float eta, const Material* material, TransportMode mode) :
//		BSSRDF(po, eta),
//		ns(po.normalGeometric),
//		ss(po.bitangent),
//		ts(po.tangent),
//		material(material),
//		mode(mode),
//		fresnel(1, eta)
//	{}
//
//	Color3f S(const Intersection& pi, const Vector3f& wi)
//	{
//		return Color3f(0);
//	}
//	Color3f Sw(const Vector3f& w) const
//	{
//		return Color3f(0);
//	}
//	Color3f Sp(const Intersection& pi) const
//	{
//		return Color3f(0);
//	};
//
//	Color3f Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) const
//	{
//		return Color3f(0);
//	}
//	Color3f Sample_Sp(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) const
//	{
//		return Color3f(0);
//	}
//	float Pdf_Sp(const Intersection& isect) const
//	{
//		return 0;
//	}
//
//	// Scattering profile gets supplied by a table
//	virtual Color3f Sr(float d) const;
//	// Return negative for failed sample (no scattering from provided channel)
//	virtual float Sample_Sr(int ch, float u) const = 0;
//	virtual float Pdf_Sr(int ch, float r) const = 0;
//	
//private:
//	const Normal3f ns;	// normal
//	const Vector3f ss;	// bitangent
//	const Vector3f ts;	// tangent
//	const Material* material;
//	const TransportMode mode;
//	const FresnelDielectric fresnel;
//};
//
//class BSSRDFTable
//{
//	BSSRDFTable() = default;
//	~BSSRDFTable() = default;
//};
//
//class TabulatedBSSRDF : public SeparableBSSRDF
//{
//public:
//	TabulatedBSSRDF(const Intersection& po, const Material* material, TransportMode mode, float eta, const Color3f& sigmaA,  const Color3f& sigmaS, const BSSRDFTable& table) :
//		SeparableBSSRDF(po, eta, material, mode) {}
//	
//	Color3f Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) override;
//	Color3f Sr(float d) const override;
//	float Sample_Sr(int ch, float u) const override;
//	float Pdf_Sr(int ch, float r) const override;
//
//private:
//	
//};