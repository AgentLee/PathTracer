#pragma once
#include <globals.h>
#include <raytracing/intersection.h>
#include "fresnel.h"
#include "bxdf.h"

class Intersection;
class Scene;
class BxDF;

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
	BSSRDF(const Intersection& po, float eta = 1);
	virtual ~BSSRDF() = default;

	virtual Color3f S(const Intersection& pi, const Vector3f& wi) = 0;
	virtual Color3f Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) = 0;
	
protected:
	const Intersection& po;
	float eta;
};

class SeparableBSSRDF : public BSSRDF
{
	friend class SeparableBSSRDFAdapter;
public:
	SeparableBSSRDF(const Intersection& po, float eta/*, const Material* material*/);
	~SeparableBSSRDF() = default;
	
	Color3f S(const Intersection& pi, const Vector3f& wi) override;
	Color3f Sample_S(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) override;

	// Bounds influence
	Color3f Sw(const Vector3f& w) const;

	// Spatial profile term
	Color3f Sp(const Intersection& pi) const;
	// Find exit point to sample after bouncing around inside the material.
	Color3f Sample_Sp(const Scene& scene, float u1, const Point2f& u2, Intersection* isect, float* pdf) const;
	float Pdf_Sp(const Intersection& isect) const;
	
	// Scattering profile gets supplied by a table
	virtual Color3f Sr(float d) const = 0;
	// Return negative for failed sample (no scattering from provided channel)
	virtual float Sample_Sr(int ch, float u) const = 0;
	virtual float Pdf_Sr(int ch, float r) const = 0;
	
private:
	const Normal3f ns;	// normal
	const Vector3f ss;	// bitangent
	const Vector3f ts;	// tangent
	//const Material* material;
	FresnelDielectric fresnel;
};

class JensenBSSRDF : public SeparableBSSRDF
{
public:
	JensenBSSRDF(const Intersection& po, /*Material* material,*/ float eta, const Color3f sigmaA, const Color3f sigmaS);
	~JensenBSSRDF() = default;
	
	Color3f Sr(float d) const override;
	float Sample_Sr(int ch, float u) const override;
	float Pdf_Sr(int ch, float r) const override;

private:
	Color3f sigmaA, sigmaS, sigmaT;
	Color3f rho;
};

class SeparableBSSRDFAdapter : public BxDF
{
public:
	SeparableBSSRDFAdapter(const SeparableBSSRDF* bssrdf);

	Color3f f(const Vector3f& woW, const Vector3f& wiW) const;

	const SeparableBSSRDF* bssrdf;
};