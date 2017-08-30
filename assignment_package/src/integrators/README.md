Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO

    //-----------------------------------------------
    // Get intersection
    //-----------------------------------------------

    Intersection intersection = Intersection();
    // Sets intersection point, normal, etc.
    bool intersects = scene.Intersect(ray, &intersection);

    Vector3f wo = -ray.direction;
    Vector3f wiL;
    Vector3f wiB;
    float pdfL;
    float pdfB;
    BxDFType type = BSDF_ALL;
    BxDFType sampledType;

    // Check to see intersection actually hits anything
    if(!intersects) {
        return Color3f(0.f);
    }

    // Check to see if it hits a light or object
    if(intersection.objectHit->material == nullptr) {
        if(depth == recursionLimit) {
            return intersection.Le(wo);
        } else {
            return Color3f(0.f);
        }
    }

    //-----------------------------------------------
    // Do Direct Lighting Integrator
    //-----------------------------------------------

    Vector3f directWi;
    Color3f directF(0.f);
    Color3f directLte(0.f);
    float directPdf;

    int lightPos = -1;

    // Pick a random light
    int numLights = scene.lights.size();

    if(numLights == 0) {
        return Color3f(0.f);
    } else {
        // Get the light position based on the sample
        lightPos = std::min((int)std::floor(sampler->Get1D() * numLights),
                                numLights - 1);


        Color3f directLi = scene.lights.at(lightPos)->Sample_Li(intersection,
                                                                sampler->Get2D(),
                                                                &directWi,
                                                                &directPdf);

        // just use f cause you don't want to sample anything. you just want a color.
        directF = intersection.bsdf->f(wo, glm::normalize(directWi), type);

        // Make sure we're uniformly sampling the light
        directPdf /= numLights;

        if(directPdf == 0.f) {
            directLte = Color3f(0.f);
        }

        bool b = intersection.ProduceBSDF();

        // Test for shadows
        Intersection isx = Intersection();
        bool shadowIntersects = scene.Intersect(intersection.SpawnRay(directWi), &isx);

        if(shadowIntersects && (isx.objectHit->areaLight != scene.lights.at(lightPos))) {
            directLte = Color3f(0.f);
        } else {
            directLte = (directF * directLi * AbsDot(directWi, intersection.normalGeometric)) / directPdf;
        }
    }

    //-----------------------------------------------
    // Do Naive Integrator
    //-----------------------------------------------

    Vector3f brdfWi;
    Color3f brdfLte(0.f);
    Color3f brdfF(0.f);

    if(depth == 0) {
        brdfLte = Color3f(0.f);
    } else {
        brdfF += intersection.bsdf->Sample_f(wo,
                                             &brdfWi,
                                             sampler->Get2D(),
                                             &pdfB,
                                             type,
                                             &sampledType);

        Ray r = intersection.SpawnRay(brdfWi);

        if(pdfB == 0.f) {
            brdfLte = Color3f(0.f);
        } else {
            brdfLte = (brdfF *Li(r, scene, sampler, --depth) * AbsDot(wiB, intersection.normalGeometric)) / pdfB;
        }
    }

    //-----------------------------------------------
    // Combine direct and naive to get full
    //-----------------------------------------------

    float brdfWeight = 0.f;

    if(lightPos == -1) {
        brdfWeight = PowerHeuristic(1, pdfB, 1, scene.lights[lightPos]->Pdf_Li(intersection, brdfWi) / scene.lights.length());
    }

    float directWeight = PowerHeuristic(1, directPdf, 1, intersection.bsdf->Pdf(wo, directWi, BSDF_ALL));

    Color3f full = brdfWeight * brdfLte + directWeight * directLte;

    return intersection.Le(wo) + full;
}