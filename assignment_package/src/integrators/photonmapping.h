#pragma once

#include "integrator.h"

class Photon
{
public:
    // Constructors
    Photon() {}
    Photon(Point3f pos, Vector3f dir, float pow, Color3f col) :
        position(pos), direction(dir), flux(pow), color(col) {}
    ~Photon() {}

    // Member variables
    Point3f position;
    Vector3f direction;
    float flux;
    Color3f color;
};

class PhotonMapping : public Integrator
{
public:
    PhotonMapping(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin using multiple importance sampling
    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const;

    // Member variables
    std::vector<Photon> directPhotonMap;
    std::vector<Photon> indirectPhotonMap;
    std::vector<Photon> causticsPhotonMap;
};
