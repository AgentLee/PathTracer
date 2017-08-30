#include "directlightingintegrator.h"

/// Light source importance sampling and evaluates the light energy that a given
/// point receives directly from the light sources
///
/// DirectLightingIntegrator will
///     - never recurse --> iterative
///     - similar to NaiveIntegrator
///         - instead of calling the BSDF's Sample_f to get wi
///           randomly pick a light from scene.lights and
///           call its Sample_Li()
///         - evaulate the rest of LTE
///
/// Divide the pdf from Sample_Li() by the number of lights
/// to ensure a randomly uniform chosen light.
/// This boosts the energy obtained from any one light
/// by a factor of N (number of lights).
Color3f DirectLightingIntegrator::Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
    //------------------//
    // Get Intersection //
    //------------------//

    Intersection intersection = Intersection();
    // Sets intersection point, normal, etc.
    bool intersects = scene.Intersect(ray, &intersection);

    Vector3f wo = -ray.direction;
    Vector3f wi;
    float pdf;
    BxDFType type = BSDF_ALL;

    // Check to see intersection actually hits anything
    if(!intersects) {
        return Color3f(0.f);
    }

    // Check to see if it hits a light
    if(intersection.objectHit->material == nullptr) {
        return intersection.Le(wo);
    }


    //-------------------------------//
    // Pick a random light to sample //
    //-------------------------------//

    int numLights = scene.lights.size();
    if(numLights == 0) {
        return Color3f(0.f);
    }

    // Get the light position based on the sample
    int lightPos = std::min((int)std::floor(sampler->Get1D() * numLights), numLights - 1);  // test Get2D()

    Color3f li = scene.lights.at(lightPos)->Sample_Li(intersection, sampler->Get2D(), &wi, &pdf);

    // make sure we're uniformly sampling the light
    pdf /= numLights;

    if(pdf == 0.f) {
        return intersection.Le(wo);
    }

    bool b = intersection.ProduceBSDF();

    // just use f cause you don't want to sample anything. you just want a color.
    Color3f f = intersection.bsdf->f(wo, glm::normalize(wi), type);

    // Test for shadows
    Intersection isx = Intersection();
    Ray shadowFeeler = intersection.SpawnRay(wi);
    bool shadowIntersects = scene.Intersect(shadowFeeler, &isx);

    if(shadowIntersects &&
       (isx.objectHit->areaLight != scene.lights.at(lightPos)))
    {
        return Color3f(0.f);
    }

    Color3f overall = (f * li * AbsDot(wi, intersection.normalGeometric)) / pdf;
    overall *= numLights;

    return overall;

//    return (f * li * AbsDot(wi, intersection.normalGeometric)) / pdf;
}
