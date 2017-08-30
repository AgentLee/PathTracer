#include "volumetricintegrator.h"

// Estimate lighting using MIS
Color3f VolumetricIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
    // Accumulated color
    Color3f L(0.f);
    // Throughput color
    Color3f beta(1.f);
    // Temporary ray for iteration
    Ray r(ray);
    // Check for specularity
    bool specularBounce = false;

    float sigmaA    = 0.000f;
    float sigmaS    = 0.008f;
    float g         = 0.f;
    Color3f mediumColor(0.5f);
    Medium medium(sigmaA, sigmaS, g, mediumColor);

    // Ray march begins at the camera
    Point3f rmOrigin = scene.camera.eye;
    Point3f rmEnd(0.f);

    while(depth != 0) {
        Vector3f    wo = -r.direction;
        BxDFType    type = BSDF_ALL;

        // Global Illumination variables
        Vector3f    giWi;
        float       giPdf;
        BxDFType    giSampledType;
        Color3f     giF(0.f);
        Color3f     gi(0.f);

        Color3f lightColor(0.f);
        Color3f inScatter(0.f);

        // Check to see if we hit anything for the ray march
        // If we do hit something then we have to set the rmEnd
        // to the intersection point.
        Intersection intersection = Intersection();
        bool intersects = scene.Intersect(r, &intersection);

        bool objectMedium = false;

        if(scene.hasMedium || (intersects && intersection.objectHit->hasMedium)) {
            if(!intersects) {
                rmEnd = Point3f(r.direction.x * 50.f, r.direction.y * 50.f, r.direction.z * 50.f);
            }
            else if(intersects && intersection.objectHit->hasMedium){
                objectMedium = true;

                // Get the out point for the object

                // Offset the point on the ray
                Point3f p = rmOrigin + (1.f * glm::normalize(r.direction));
                // Make a new ray
                Ray subray = Ray(p, glm::normalize(r.direction));

                // Assuming no overlapping objects, get the out point
                Intersection isx = Intersection();
                bool hitsObject = scene.Intersect(subray, &isx);

                // If we hit an object and it's the same object then we have the out point
                if(hitsObject && isx.objectHit == intersection.objectHit) {
                    // Reset the ray origin to the entry point
                    rmOrigin = intersection.point + glm::normalize(intersection.normalGeometric);
                    rmEnd = isx.point + glm::normalize(isx.normalGeometric);
                }
            }
            else {
                rmEnd = intersection.point + glm::normalize(intersection.normalGeometric) * 0.01f;
            }

            //-----------------------------------------------------
            // Ray march through the volume
            //-----------------------------------------------------
            // We're assuming that the entire scene is filled with
            // a medium. Because of this we'll ray march the ray
            // from the camera to every object hit in the scene.
            // From that point we ray march to the light.
            //-----------------------------------------------------

            // Set up variables for ray march
            float stepSize = 1.f;
            // Get the distance between the origin and the object hit
            // rmOrigin should initially be at the camera
            float tmax = glm::distance(rmOrigin, rmEnd);
            // t gets incremented at the end of each iteration to
            // keep marching along the ray.
            float t = 0.f;

            // Transmissivity
            // 0 for opaque
            // 1 for clear
            // Use this to help determine the end of the ray march
            float Tr = 1.f;     // Accumulated
            float TrToPoint = 1.f;
            float TrToLight = 1.f;

            // In-scattered Radiance
            Color3f Ls(0.f);
            // Emitted Radiance
            Color3f Le(0.f);
            // Incident Radiance
            Color3f Li(0.f);
            // Phase Scattering
            Color3f fp(0.f);
            // Spherical sampling
            float pdf = Inv4Pi;

            // Ray march
            for(t = 0.f; t < tmax; t += stepSize) {
                // Get point on the ray
                Vector3f d  = glm::normalize(r.direction);
                Point3f p   = rmOrigin + (t * d);

                // If the transmissivity gets less than 0,
                // then the point is beyond visibility so
                // we'll break out of the ray march.
                if(TrToPoint < 0.f) {
                    break;
                }

                // Get the transmissivity for the point.
                // Only update after the first ray march.
                if(t > 0.f) {
                    TrToPoint = medium.Transmittance(rmOrigin, p);
                }

                Ls = Color3f(0.f);

                // To determine the Ls term we have to do
                // spherical sampling at the ray marched point.
                int M = 25;
                for(int j = 0; j < M; j++) {
                    // Get a random point on on the sphere and use this as the direction
                    Point3f samplePoint = glm::normalize(WarpFunctions::squareToSphereUniform(sampler->Get2D()));
                    giWi = samplePoint;

                    // The phase function replaces sampling the BRDF
                    fp = Color3f(medium.PhaseHG(p, samplePoint));

                    // Using this samplePoint as the direction for a ray
                    Ray pToSamplePoint(p, samplePoint);

                    Li = Color3f(0.f);

                    // Check to see if the ray hits an object
                    Intersection intersection = Intersection();
                    bool hitsObj = scene.Intersect(pToSamplePoint, &intersection);
                    // Check to see if the object hit is a light
                    if(hitsObj) {
                        // Solve for the Li term
                        if(intersection.objectHit->material == nullptr) {
                            Le = intersection.Le(-samplePoint);

                            // Get the transmissivity from the point to the light
                            TrToLight = medium.Transmittance(p, intersection.point);

                            // Solve for Li
                            Li = TrToLight * Le;
                        }
                    }

                    // Solve for Ls
                    Ls += (fp * Li) / pdf;
                }

                // Average the Ls term
                Ls /= M;

                // Add to L
                inScatter += (Color3f(TrToPoint) * medium.sigma_s * Ls);
            }

            beta *= Color3f(medium.Transmittance(r.origin, intersection.point) * medium.sigma_s);
            L += inScatter;
        }

        if(objectMedium) {
            depth--;

            continue;
        }

        // Full Lighting

        if(!intersects) {
            break;
        }

        bool producedBSDF = intersection.ProduceBSDF();
        if(!producedBSDF) {
            if (depth == recursionLimit || specularBounce) {
                if(scene.hasMedium) {
                    L += beta * Color3f(medium.Transmittance(r.origin, intersection.point)) * medium.sigma_a * intersection.Le(wo);
                } else {
                    L += beta * intersection.Le(wo);
                }
            } else {
                for(int i = 0; i < scene.lights.size(); i++) {
                    if(scene.hasMedium) {
                        L += beta * Color3f(medium.Transmittance(r.origin, intersection.point)) * medium.sigma_a * scene.lights.at(i)->Le(r);
                    } else {
                        L += beta * scene.lights.at(i)->Le(r);
                    }
                }
            }

            break;
        }

        // As long as there wasn't a specular bounce we don't have to worry about MIS
        if(!specularBounce) {
            // Use MIS to estimate the lighting on the surface
            lightColor = EstimateDirectLighting(r, scene, sampler, intersection);
        }

        L += beta * lightColor;

        //-----------------------------------------------------
        // Global Illumination
        //-----------------------------------------------------
        float prob = sampler->Get1D();

        giF = intersection.bsdf->Sample_f(wo, &giWi, sampler->Get2D(), &giPdf, type, &giSampledType);

        if (fequal(giPdf, 0.f) || IsBlack(giF)) {
            break;
        }

        gi = (giF * AbsDot(giWi, intersection.normalGeometric)) / giPdf;

        // Update throughput for next bounce
        beta *= gi;

        specularBounce = (giSampledType & BSDF_SPECULAR);

        rmOrigin = rmEnd;
        r = intersection.SpawnRay(giWi);

        // Russian Roulette Termination
        if (depth < recursionLimit - 3) {
            float comp = glm::max(glm::max(beta.x, beta.y), beta.z);

            if (comp < prob) {
                return L;
            } else {
                beta /= (1.f - prob);
            }
        }

        depth--;
    }

    return L;
}
