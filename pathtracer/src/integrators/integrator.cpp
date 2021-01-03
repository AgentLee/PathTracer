#include "integrator.h"

void Integrator::run()
{
    Render();
}

void Integrator::Render()
{
    std::vector<Point2f> highVariancePixels;

    // Compute the bounds of our sample, clamping to screen's max bounds if necessary
    // Instantiate a FilmTile to store this thread's pixel colors
    std::vector<Point2i> tilePixels = bounds.GetPoints();
    // For every pixel in the FilmTile:
    for(Point2i pixel : tilePixels)
    {
        ///Uncomment this to debug a particular pixel within this tile
//        if(pixel.x != 0 && pixel.y != 138)
//        {
//            continue;
//        }

        Color3f pixelColor(0.f);
        Color3f pixelColorSq(0.f);
        // Ask our sampler for a collection of stratified samples, then raycast through each sample
        std::vector<Point2f> pixelSamples = sampler->GenerateStratifiedSamples();

        int count = 0;
        bool highVariance = true;
        while(highVariance) {
            for(Point2f sample : pixelSamples)
            {
                sample = sample + Point2f(pixel); // _sample_ is [0, 1), but it needs to be translated to the pixel's origin.

                // Generate a ray from this pixel sample
                Ray ray = camera->Raycast(sample);

                // Get the L (energy) for the ray by calling Li(ray, scene, tileSampler, arena)
                // Li is implemented by Integrator subclasses, like DirectLightingIntegrator
                Color3f compoundEnergy(0.f);
                Color3f L = Li(ray, *scene, sampler, recursionLimit, compoundEnergy);

                // Accumulate color in the pixel
                pixelColor += L;
                pixelColorSq += Color3f(pow(L.x, 2), pow(L.y, 2), pow(L.z, 2));
            }

            // Average all samples' energies
            pixelColor /= (pixelSamples.size());
            pixelColorSq /= pixelSamples.size();

            Color3f variance = pixelColorSq - Color3f(pow(pixelColor.x, 2), pow(pixelColor.y, 2), pow(pixelColor.z, 2));

            // Need to tweak this later
            if(variance.x <= 5.f && variance.y <= 5.f && variance.z <= 5.f)  {
                highVariance = false;
            }

            // 8 seems like a good resample amount
            if(count >= 8) {
                break;
            }

            pixelSamples = sampler->GenerateStratifiedSamples();
            count++;
        }

        film->SetPixelColor(pixel, glm::clamp(pixelColor, 0.f, 1.f));
    }

    //We're done here! All pixels have been given an averaged color.
}

Color3f Integrator::EstimateDirectLighting(const Ray &r, const Scene &scene, std::shared_ptr<Sampler> &sampler, Intersection &intersection) const
{
    Color3f     lightColor(0.f);

    Vector3f    wo = -r.direction;
    BxDFType    type = BSDF_ALL;

    // Light MIS variables
    Vector3f    lightWi;
    float       lightPdf;
    Color3f     lightF(0.f);
    Color3f     lightLte(0.f);
    int         lightPos    = -1;
    float       lightWeight = 0.f;

    // BSDF MIS variables
    Vector3f    bsdfWi;
    float       bsdfPdf;
    BxDFType    sampledType;
    Color3f     bsdfF(0.f);
    Color3f     bsdfLte(0.f);
    float       bsdfWeight = 0.f;

    //-----------------------------------------------------
    // Light MIS
    //-----------------------------------------------------
    int numLights = scene.lights.length();
    lightPos = std::min((int)std::floor(sampler->Get1D() * numLights), numLights-1);
    Color3f lightLi = scene.lights.at(lightPos)->Sample_Li(intersection, sampler->Get2D(), &lightWi, &lightPdf);

    if(lightPdf > 0.f && !IsBlack(lightLi)) {
        lightF = intersection.bsdf->f(wo,lightWi, type);

        Intersection isx = Intersection();
        Ray shadowFeeler = intersection.SpawnRay(lightWi);
        bool inShadow = scene.Intersect(shadowFeeler, &isx);

        if (inShadow) {
            if (isx.objectHit->areaLight == scene.lights.at(lightPos)) {
                lightLte = (lightF * lightLi * AbsDot(lightWi, intersection.normalGeometric)) / lightPdf;
            }
        } else {
            for(int i = 0; i < scene.lights.size(); i++) {
                if(scene.lights.at(i)->infiniteLight) {
                    lightLte = scene.lights.at(i)->L(isx, -lightWi);
                }
            }
        }

        lightWeight = PowerHeuristic(1, lightPdf, 1, intersection.bsdf->Pdf(wo, lightWi, type));
    }

    //-----------------------------------------------------
    // BSDF MIS
    //-----------------------------------------------------
    bsdfF = intersection.bsdf->Sample_f(wo,&bsdfWi,sampler->Get2D(), &bsdfPdf, type, &sampledType);

    if(bsdfPdf > 0.f && !IsBlack(bsdfF)) {
        Intersection isx = Intersection();
        Ray ray = intersection.SpawnRay(bsdfWi);
        bool hitsLight = scene.Intersect(ray, &isx);

        if (hitsLight) {
            if (isx.objectHit->areaLight == scene.lights.at(lightPos)) {
                bsdfLte = (bsdfF * scene.lights.at(lightPos)->L(isx, -bsdfWi) * AbsDot(bsdfWi, intersection.normalGeometric)) / bsdfPdf;
                bsdfWeight = PowerHeuristic(1, bsdfPdf, 1, scene.lights[lightPos]->Pdf_Li(intersection, bsdfWi));
            }
        }
    }

    lightColor = ((lightWeight * lightLte) + (bsdfWeight * bsdfLte)) * float(numLights);
	lightColor = WHITE / 5.0f;
	//if(IsBlack(lightColor))
	//	printf("debug");
	
    return lightColor;
}

void Integrator::ClampBounds()
{
    Point2i max = bounds.Max();
    max = Point2i(std::min(max.x, film->bounds.Max().x), std::min(max.y, film->bounds.Max().y));
    bounds = Bounds2i(bounds.Min(), max);
}
