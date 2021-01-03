#include "fulllightingintegrator.h"

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
    // Accumulated color
    Color3f L(0.f);
    // Throughput color
    Color3f beta(1.f);
    // Temporary ray for iteration
    Ray r(ray);
    // Check for specularity
    bool specularBounce = false;

    while (depth != 0) {
        Vector3f    wo = -r.direction;
        BxDFType    type = BSDF_ALL;

        Color3f lightColor(0.f);

        // Light MIS variables
        Vector3f    lightWi;
        float       lightPdf;
        Color3f     lightF(0.f);
        Color3f     lightLte(0.f);
        int         lightPos    = -1;
        float       lightWeight = 0.f;

        // Global Illumination variables
        Vector3f    giWi;
        float       giPdf;
        BxDFType    giSampledType;
        Color3f     giF(0.f);
        Color3f     gi(0.f);

        // Check to see if the ray hits anything.
        Intersection intersection = Intersection();
        bool intersects = scene.Intersect(r, &intersection);
        if (!intersects) {
            // Environment Lighting
//            for(int i = 0; i < scene.lights.size(); i++) {
//                if(scene.lights.at(i)->infiniteLight) {
//                    L += beta * scene.lights.at(i)->Sample_Li(intersection,
//                                                      sampler->Get2D(),
//                                                      &wo,
//                                                      &lightPdf);
//                }
//            }

            break;
        }

        // Check to see if the ray hits an object with a BSDF.
    	// A null BSDF means that the surface has no effect from the light.
        bool producedBSDF = intersection.ProduceBSDF();
        if (!producedBSDF) {
            if (depth == recursionLimit || specularBounce) {
                L += beta * intersection.Le(wo);
            } else {
                for(int i = 0; i < scene.lights.size(); i++) {
                    L += beta * scene.lights.at(i)->Le(r);
                }
            }

            break;
        }

        // As long as there wasn't a specular bounce we don't have to worry about MIS
        if(!specularBounce) {
            // Use MIS to estimate the lighting on the surface
            lightColor = EstimateDirectLighting(r, scene, sampler, intersection);
        }

        // Update the overall color
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

        specularBounce = (giSampledType & BSDF_SPECULAR) != 0;

        // Update the ray for the next bounce
        r = intersection.SpawnRay(giWi);

		// TODO: also check for flags & BSDF_TRANSMISSION
		if (intersection.bssrdf != nullptr)
		{
			Intersection pi = Intersection();

			// Which pdf to use? 
			auto S = intersection.bssrdf->Sample_S(scene, sampler->Get1D(), sampler->Get2D(), &pi, &giPdf);
			if(IsBlack(S) || giPdf == 0.0f)
				break;

			// TODO: remove
			if(pi.bsdf == nullptr)
				return GREEN;
			
			L += beta * EstimateDirectLighting(r, scene, sampler, pi);

			auto f = pi.bsdf->Sample_f(pi.wo, &giWi, sampler->Get2D(), &giPdf, BSDF_ALL, &giSampledType);
			if(IsBlack(f) || giPdf == 0.0f)
				break;

			beta *= f * AbsDot(giWi, pi.normalGeometric) / giPdf;

			specularBounce = (giSampledType & BSDF_SPECULAR) != 0;

			r = pi.SpawnRay(giWi);
			
			return S;
		}
    	
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
