#pragma once
#include <QRunnable>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/scene.h>
#include <globals.h>
#include <samplers/sampler.h>
#include <scene/lights/light.h>

/// This class is analogous to RenderTask() in RayTracer

// An interface for rendering scenes by evaluating the Light Transport Equation
class Integrator : public QRunnable
{
public:
    Integrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : scene(s), camera(&(s->camera)), film(&(s->film)), sampler(sampler),
          bounds(bounds), recursionLimit(recursionLimit)
    {
        ClampBounds();
    }

    virtual ~Integrator(){}

    virtual void run();     // A function declared in the QRunnable class that we must implement
                            // in order for our class to be compatible with a QThreadPool.
                            // This function should iterate over all the pixels this Integrator
                            // is supposed to handle and trace a ray through each of them, then
                            // set the pixel of the film to the correct color.

    /// Iterates over all the pixels and generates a collection of samples within a pixel.
    /// Rays are cast through each sample to evaluate the energy from the camera and back
    /// to the pixel.
    ///
    /// Li() is invoked to evaluate energy (not in this class).
    virtual void Render(); // Called from run() or from MyGL depending on whether we're multithreading our render.


    // Evaluate the energy transmitted along the ray back to
    // its origin, e.g. the camera or an intersection in the scene
    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const = 0;
    // Estimate lighting using MIS
    Color3f EstimateDirectLighting(const Ray &r, const Scene &scene, std::shared_ptr<Sampler> &sampler, Intersection &intersection) const;
    // Clamp the upper end of our bounds to not go past the edge of the film.
    void ClampBounds();

protected:
    Scene const * const scene;
    Camera const * const camera;			// A pointer to the Camera instance stored in MyGL.
    Film * const film;						// A pointer to the Film instance stored in MyGL.
    std::shared_ptr<Sampler> sampler;       // A pointer to the Sampler that we will use to generate pixel samples for this thread.

    Bounds2i bounds;                  // The min and max bounds of the film to which this thread renders
    int recursionLimit;
};
