#include "photonmapping.h"

Color3f PhotonMapping::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
    return Color3f(0.f);
}
