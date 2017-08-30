#pragma once
#include "integrator.h"

class FullLightingIntegrator : public Integrator
{
public:
    FullLightingIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin using multiple importance sampling
    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const;

    bool depthCheck(int depth);
    bool russianRoulette(Color3f &beta, float prob, int depth) const;
    bool isInShadow(Intersection &intersection, Vector3f &wi, int &lightPos) const;
};
