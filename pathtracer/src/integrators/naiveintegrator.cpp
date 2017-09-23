#include "naiveintegrator.h"

/// Recursively evaluate the energy emitted from the scene along some ray back to the camera.
/// Find the intersection of the input ray and evaluate the LTE for the intersection.
///
/// The ray's origin is either the camera or the point of intersection
///
/// Helpful functions:
///     Scene::Intersect()
///     Intersection::Le()
///     Intersection::ProduceBSDF()
///     BSDF::Sample_f()
///     Intersection::SpawnRay()
Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
    Color3f f(0.f);

    Intersection intersection = Intersection();
    bool intersects = scene.Intersect(ray, &intersection);

    // Took out the pointer stuff here.
    // Got scared from being stuck on intersection crashing.
    Vector3f wo = -ray.direction;
    Vector3f wi;       // tbd in sample_f
    float pdf;
    BxDFType type = BSDF_ALL;
    BxDFType sampledType;

    // Ordering of conditions is extremely important.
    // This was the bulk of the crashing.

    // If intersection doesn't exist, return black.
    if(!intersects) {
        return Color3f(0.f);
    }

    // If intersection hits a light, return Le
    if(intersection.objectHit->material == nullptr) {
        return intersection.Le(wo);
    }

    // If depth is 0, return Li
    if(depth == 0) {
        return f;
    }

    // Sets the intersection BSDF
    intersection.ProduceBSDF();

    // sampler
    f += intersection.bsdf->Sample_f(wo, &wi, sampler->Get2D(), &pdf, type, &sampledType);

    // Removes the black dots.
    // Prevent divide by 0 error.
    if(pdf == 0.f) {
        return Color3f(0.f);
    }

    // Want to use wi because you don't want to trace a ray back into the camera.
    // That's just silly.
    // You want to go away from the camera into something else, so wi.
    Ray r = intersection.SpawnRay(wi);

    // --depth because you want to decrease before passing through.
    // I forgot intro to cs concepts. womp
    return intersection.Le(wo) + (f * Li(r, scene, sampler, --depth, compoundEnergy) * AbsDot(wi, intersection.normalGeometric)) / pdf;
}
