#pragma once
#include "integrator.h"
#include "scene/medium.h"

// VolumetricIntegrator should follow the same construct as FullLighting.
class VolumetricIntegrator : public Integrator
{
public:
    VolumetricIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const;
};
