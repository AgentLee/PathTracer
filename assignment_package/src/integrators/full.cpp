#include "fulllightingintegrator.h"

#define BLACK Color3f(0.f);
#define WHITE Color3f(1.f);

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{

    // Instantiate 2 ray colors
    //      one black to return
    //      one white to determine when the ray path terminates against russianRoulette
    // Use a while loop that compares the current depth to 0
    //      check if russianRoulette to break
    // return the accumulated ray color

    // L holds the accumulated radiance
    Color3f L(0.f);
    // beta is the product of BSDF values and cosine terms
    // for the vertices generated so far and divide it by the PDF.
    // beta & direct lighting at the final vertex gives you the
    // contribution for the path.
    Color3f beta(1.f);
    // r holds the next ray to be traced to extend the path
    Ray r(ray);
    // specularBounce holds if the last outgoing path direction
    // sampled was from a specular reflection.
    bool specularBounce = false;

    while(depth >= 0)
    {
        //-----------------------------------------------
        // Variables
        //-----------------------------------------------
        //
        // These are pretty global for direct and BSDF sampling
        Vector3f    wo = -r.direction;
        // wi and pdf to be set at direct and BSDF sampling
        BxDFType    type = BSDF_ALL;
        BxDFType    sampledType;        // to be set in BSDF sampling

        //-----------------------------------------------
        // Check intersection
        //-----------------------------------------------
        Intersection    intersection = Intersection();
        bool            intersects = scene.Intersect(r, &intersection);

        //-----------------------------------------------
        // Add emitted light
        //-----------------------------------------------
        //
        // Emission is usually ignored if a ray hits an
        // emissive object. This is because the previous
        // path already accounted for it with direct
        // lighting. There are two cases for this though.
        //      1. The very first intersection point is emissive
        //      2. The sampled direction from the last path
        //         was from a specular BSDF
        // With the second case, direct lighting at the
        // previous step cannot account for the emission.
        // Therefore we have to take care of that here.
        if(depth == recursionLimit || specularBounce) {
            // The path throughput weight should eb mutliplied
            // by the radiance at the current path if there
            // was an intersection and when emission is to be
            // accounted for. Otherwise the just take into
            // account the lights in the scene.
            if(intersects) {
                L += beta * intersection.Le(wo);
            } else {
                for(const auto &light : scene.lights) {
                    L += beta * light->Le(r);
                }
            }
        }

        //-----------------------------------------------
        // End the path
        //-----------------------------------------------
        // If we don't hit anything in the scene or if
        // we hit below 0 we're just going to break out
        // and return what we got for L.
        if(!intersects || depth <= 0) {
            break;
        }

        //-----------------------------------------------
        // BSDF set up
        //-----------------------------------------------
        //
        // We have to find the BSDF before sampling the light.
        // If we don't get a material then it has no effect
        // on the light.
        bool producedBSDF = intersection.ProduceBSDF();
        if(!producedBSDF) {
            r = intersection.SpawnRay(r.direction);
            depth--;
            continue;
        }

        //-----------------------------------------------
        // Light Sampling
        //-----------------------------------------------
        //
        // Use MIS to evaluate direct lighting the ray
        // recieves from a randomly chosen light source.
        //
        // This value will be ONE term.
        //
        // If the BSDF generated ray does NOT hit the selected
        // light, its contribution will be black. It should NOT
        // have any global illumination or light from other sources.
        //
        // Make sure to scale the contribution by the number
        // of light sources.
        //
        // Treat specular surfaces as black.
        int lightPos = -1;
        int numLights = scene.lights.size();
        Color3f lightSample = BLACK;

        if(numLights == 0) {
            lightSample = BLACK;
        } else {
            // Get the light position
            lightPos = std::min((int)std::floor(sampler->Get1D() * numLights),
                                                numLights - 1);

            // Check to see if we hit the selected light
            if(intersection.objectHit->areaLight == scene.lights.at(lightPos)) {
                Vector3f wi;
                float lightPdf;

                Color3f li = scene.lights.at(lightPos)->Sample_Li(intersection,
                                                                  sampler->Get2D(),
                                                                  &wi,
                                                                  &lightPdf);

                // Make sure we're uniformly sampling the light
                lightPdf /= numLights;

                if(lightPdf == 0.f) {
                    lightSample = BLACK
                } else {
                    Color3f f = intersection.bsdf->f(wo, glm::normalize(wi), type);

                    Intersection isx = Intersection();
                    Ray shadowFeeler = intersection.SpawnRay(wi);
                    bool shadowIntersects = scene.Intersect(shadowFeeler, &isx);

                    if(shadowIntersects &&
                        (isx.objectHit->areaLight != scene.lights.at(lightPos)))
                    {
                        lightSample = BLACK;
                    } else {
                        lightSample = (f * li * AbsDot(wi, intersection.normalGeometric)) / lightPdf;
                    }
                }
            } else {
                lightSample = BLACK;
            }
        }

        L += (beta * lightSample);
        L *= numLights;

        //-----------------------------------------------
        // BSDF Sampling -
        // Ray bounce and global illumination
        //-----------------------------------------------
        //
        // This part is entirely separate from light sampling.
        //
        // Use a new uniform 2D random variable to get a new
        // BSDF wi using Sample_f.
        //
        // Multiply the color from Sample_f with the THROUGHPUT
        // along with the absolute-dot-product and 1/pdf.
        // This compounds the inherent material colors that the
        // ray has bounced off so far.
        // Be sure to multiply it with the direct lighting
        // term and add it to the final color.
        //
        // Also update the ray so that it starts at the current
        // intersection and travel in the direction of wi.
        Vector3f wi;
        float pdf;
        Color3f bsdfSample = BLACK;

        // Something in here is fucked up
        if(depth == 0) {
            bsdfSample = BLACK;
        } else {
            Color3f f = intersection.bsdf->Sample_f(wo,
                                                    &wi,
                                                    sampler->Get2D(),
                                                    &pdf,
                                                    type,
                                                    &sampledType);


            if(f.x == 0.f & f.y == 0.f && f.z == 0.f) {
                break;
            }

            if(pdf == 0.f) {
                break;
            }

            beta *= f * AbsDot(wi, intersection.normalGeometric) / pdf;
            beta *= lightSample;

        }

        specularBounce = (sampledType & BSDF_SPECULAR) != 0;

        r = intersection.SpawnRay(wi);

        float prob = sampler->Get1D();

        if(depth <= (recursionLimit - 3)) {
            float q = std::max(0.5f, 1 - beta.y);

            if(prob < q) {
                break;
            }

            beta /= (1 - q);
        }

        //-----------------------------------------------
        // Direct Lighting continued
        //-----------------------------------------------
        //
        // Each ray computed from Direct Lighting should NOT
        // incorporate Le. Le should only be incorporated when
        // the ray came directly from the camera or from a
        // perfectly specular surface.

        //-----------------------------------------------
        // Russian Roulette
        //-----------------------------------------------
        //
        // Divide the throughput by the max component of
        // the throughput

        depth--;
    }

    return L;
}

// Too saturated
Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{

   // Instantiate 2 ray colors
   //      one black to return
   //      one white to determine when the ray path terminates against russianRoulette
   // Use a while loop that compares the current depth to 0
   //      check if russianRoulette to break
   // return the accumulated ray color

   Color3f L(0.f);     // Accumulated
   Color3f beta(1.f);  // Throughput
   Color3f BLACK(0.f);
   Ray r(ray);

   while(depth >= 0)
   {
       //-----------------------------------------------
       // Variables
       //-----------------------------------------------
       Vector3f wo = -r.direction;
       // wi and pdf to be set in their respective functions
       BxDFType type = BSDF_ALL;
       BxDFType sampledType;


       //-----------------------------------------------
       // Check intersection
       //-----------------------------------------------
       Intersection intersection = Intersection();
       bool intersects = scene.Intersect(r, &intersection);

       // Go to the next iteration since you didn't hit anything
       if(!intersects) {
           L += BLACK;
           depth--;
           continue;
       }

       bool b = intersection.ProduceBSDF();

       //-----------------------------------------------
       // Light Sampling
       //-----------------------------------------------
       //
       // Use MIS to evaluate direct lighting the ray
       // recieves from a randomly chosen light source.
       //
       // This value will be ONE term.
       //
       // If the BSDF generated ray does NOT hit the selected
       // light, its contribution will be black. It should NOT
       // have any global illumination or light from other sources.
       //
       // Make sure to scale the contribution by the number
       // of light sources.
       //
       // Treat specular surfaces as black.

       Vector3f    lightWi;
       float       lightPdf;
       Color3f     lightF(0.f);
       Color3f     lightLte(0.f);

       int lightPos = -1;
       int numLights = scene.lights.size();

       if(numLights == 0) {
           lightLte = BLACK;
       } else {

           lightPos = std::min((int)std::floor(sampler->Get1D() * numLights), numLights - 1);


           // Check to see if we hit the selected light
           if(scene.lights.at(lightPos) == intersection.objectHit->areaLight) {
               Color3f directLi = scene.lights.at(lightPos)->Sample_Li(intersection,
                                                                       sampler->Get2D(),
                                                                       &lightWi,
                                                                       &lightPdf);

               lightPdf /= numLights;

               if(lightPdf == 0.f) {
                   lightLte = BLACK;
               } else {
                   lightF = intersection.bsdf->f(wo,
                                                 glm::normalize(lightWi),
                                                 type);

                   // Test for shadows
                   Intersection isx = Intersection();
                   Ray shadowFeeler = intersection.SpawnRay(lightWi);
                   bool shadowIntersects = scene.Intersect(shadowFeeler, &isx);

                   if(shadowIntersects &&
                      (isx.objectHit->areaLight != scene.lights.at(lightPos)))
                   {
                       lightLte = BLACK;
                   } else {
                       lightLte = (lightF * directLi * AbsDot(lightWi, intersection.normalGeometric)) / lightPdf;
                       lightLte *= numLights;
                   }
               }
           }
           // Doesn't hit so contribution is black
           else {
               lightLte = BLACK;
           }
       }


       //-----------------------------------------------
       // BSDF Sampling -
       // Ray bounce and global illumination
       //-----------------------------------------------
       //
       // This part is entirely separate from light sampling.
       //
       // Use a new uniform 2D random variable to get a new
       // BSDF wi using Sample_f.
       //
       // Multiply the color from Sample_f with the THROUGHPUT
       // along with the absolute-dot-product and 1/pdf.
       // This compounds the inherent material colors that the
       // ray has bounced off so far.
       // Be sure to multiply it with the direct lighting
       // term and add it to the final color.
       //
       // Also update the ray so that it starts at the current
       // intersection and travel in the direction of wi.

       Vector3f    bsdfWi;
       float       bsdfPdf;
       Color3f     bsdfF(0.f);
       Color3f     bsdfLte(0.f);

       if(depth == 0) {
           bsdfLte = BLACK;
       } else {
           bsdfF += intersection.bsdf->Sample_f(wo,
                                                &bsdfWi,
                                                sampler->Get2D(),
                                                &bsdfPdf,
                                                type,
                                                &sampledType);


           if(bsdfPdf == 0.f) {
               bsdfLte = BLACK;
           } else {
               beta *= bsdfF * AbsDot(bsdfWi, intersection.normalGeometric) / bsdfPdf;
               beta *= lightLte;
           }

           // update ray
           r = intersection.SpawnRay(bsdfWi);
       }

       L += beta;

       //-----------------------------------------------
       // Direct Lighting continued
       //-----------------------------------------------
       //
       // Each ray computed from Direct Lighting should NOT
       // incorporate Le. Le should only be incorporated when
       // the ray came directly from the camera or from a
       // perfectly specular surface.

       //-----------------------------------------------
       // Russian Roulette
       //-----------------------------------------------
       //
       // Divide the throughput by the max component of
       // the throughput

       depth--;
   }

}



Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
   Color3f L(0.f), beta(1.f);
   Ray r(ray);

   while(depth > -1)
   {
       // Get intersection
       Intersection intersection = Intersection();
       bool intersects = scene.Intersect(r, &intersection);

       if(depth == 0)
       {
           if(intersects)
           {
               L += beta * intersection.Le(-r.direction);
           }
           else
           {
               for(const auto &light : scene.lights)
               {
                   L += beta * light->Le(r);
               }
           }
       }

       if(!intersects)
       {
           break;
       }

       bool producedBSDF = intersection.ProduceBSDF();
       if(!producedBSDF)
       {
           r = intersection.SpawnRay(r.direction);
           depth--;
           continue;
       }

       // Sample Light
       Vector3f    lightWo = -r.direction;
       Vector3f    lightWi;
       float       lightPdf;
       Color3f     lightF(0.f);
       Color3f     lightLte(0.f);

       int lightPos = -1;

       int numLights = scene.lights.size();

       if(numLights == 0)
       {
           lightLte = Color3f(0.f);
       }
       else
       {
           lightPos = std::min((int)std::floor(sampler->Get1D() * numLights),
                                              numLights - 1);

           Color3f lightLi = scene.lights.at(lightPos)->Sample_Li(intersection,
                                                                  sampler->Get2D(),
                                                                  &lightWi,
                                                                  &lightPdf);

           lightPdf /= numLights;

           if(lightPdf == 0.f)
           {
               lightLte = Color3f(0.f);
           }
           else
           {
               lightF = intersection.bsdf->f(lightWo, glm::normalize(lightWi), BSDF_ALL);

               Intersection isx = Intersection();
               Ray shadowFeeler = intersection.SpawnRay(lightWi);
               bool shadowIntersects = scene.Intersect(shadowFeeler, &isx);

               if(shadowIntersects &&
                  (isx.objectHit->areaLight != scene.lights.at(lightPos)))
               {
                   lightLte = Color3f(0.f);
               }
               else
               {
                   lightLte = (lightF * lightLi * AbsDot(lightWi, intersection.normalGeometric)) / lightPdf;
               }
           }
       }

//        L += lightLte;

       // Sample BSDF
       Vector3f    wo = -r.direction;
       Vector3f    wi;
       float       pdf;
       BxDFType    sampledType;
       Color3f     f(0.f);
       Color3f     bsdfLte(0.f);

       float prob = sampler->Get1D();
       if(russianRoulette(compoundEnergy, prob, depth))
       {
//            bsdfLte = Color3f(0.f);
           break;
       }
       else
       {
           f = intersection.bsdf->Sample_f(wo,
                                           &wi,
                                           sampler->Get2D(),
                                           &pdf,
                                           BSDF_ALL,
                                           &sampledType);


           r = intersection.SpawnRay(wi);

           compoundEnergy = (compoundEnergy * f * AbsDot(wi, intersection.normalGeometric) / pdf) / (1 - prob);

           if((f.x == 0.f && f.y == 0.f && f.z == 0.f) || pdf == 0.f)
           {
               break;
           }
               beta *= f * AbsDot(wi, intersection.normalGeometric) / pdf;
       }

       float bsdfWeight = 1.f;
       float lightWeight = 0.f;

       if(lightPos != -1) {
           bsdfWeight = PowerHeuristic(1,
                                       pdf,
                                       1,
                                       scene.lights.at(lightPos)->Pdf_Li(intersection, wi) / scene.lights.length());

           lightWeight = PowerHeuristic(1,
                                        lightPdf,
                                        1,
                                        intersection.bsdf->Pdf(wo, lightWi, BSDF_ALL));
       }

       Color3f full = bsdfWeight * bsdfLte + lightWeight * lightLte;

       L += intersection.Le(wo) + full;

       depth--;
   }

   return L;
}



// Correct Global Illumination
//
// Instantiate an accumulated ray color as black.
// Instantiate an accumulated ray throughput color as white.
// Throughput is used to determine when the ray path
// terminates with Russian Roulette.
//
// while depth > 0
//      if russianRoulette
//          break
//      else
//          calculate accumulated ray color
//
// return accumulated ray color
//Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
//{
//    // TODO

//    // PBRT p876
//    // Full Radiance
//    Color3f L(0.f);
//    // Product of the BSDF values and cosine terms for
//    // the vertices generated so far divided by the PDF.
//    Color3f beta(1.f);
//    // Need to determine if the last outgoing path
//    // direction was from specular reflection
//    bool specularBounce = false;
//    bool firstIter = true;
//    // Holds the next ray to be traced
//    // Set to the passed in ray.
//    Ray r(ray);

//    while(depth > 0)
//    {
//        //-----------------------------------------------
//        // Intersect ray with scene and store intersection
//        //-----------------------------------------------
//        Intersection intersection = Intersection();
//        bool intersects = scene.Intersect(r, &intersection);

//        //-----------------------------------------------
//        // Possibly add emitted light at intersection
//        //----------------------------------------------
//        if(firstIter || specularBounce) {
//            // Add emitted light at path or from environment
//            // The throughput weight must be multiplied with
//            // the radiance from the lights in the scene or
//            // at the intersection point.
//            if(intersects) {
//                L += beta * intersection.Le(-r.direction);
//            } else {
//                for(int lightPos = 0; lightPos < scene.lights.size(); lightPos++) {
//                    L += beta * scene.lights.at(lightPos)->Le(r);
//                }
//            }
//        }

//        //----------------------------------------------
//        // Terminate path if ray escaped or we reached the depth
//        // The depth check shouldn't happen
//        //----------------------------------------------
//        if(!intersects || depth <= 0) {
//            break;
//        }

//        //----------------------------------------------
//        // Compute scattering functions and skip over medium boundaries
//        //----------------------------------------------
//        bool producedBSDF = intersection.ProduceBSDF();
//        // If no BSDF is created then the surface has no effect on
//        // the light. We just spawn a new ray and go step through.
//        if(!producedBSDF) {
//            r = intersection.SpawnRay(r.direction);
//            depth--;
//            continue;
//        }

//        //----------------------------------------------
//        // Direct Lighting
//        //----------------------------------------------
//        // Pick a random light
//        int lightPos = -1;
//        int numLights = scene.lights.size();

//        if(numLights == 0) {
//            L += Color3f(0.f);
//        } else {
//            Vector3f    wo = -r.direction;
//            Vector3f    wi;
//            float       pdf;
//            Color3f     f;
//            Color3f     li;
//            BxDFType    type = BSDF_ALL;


//            // Get the light position using the sampler
//            lightPos = std::min((int)std::floor(sampler->Get1D() * numLights),
//                                               numLights - 1);

//            // Explanation for numLights p855
//            li = (float)numLights *
//                 scene.lights.at(lightPos)->Sample_Li(intersection,
//                                                      sampler->Get2D(),
//                                                      &wi,
//                                                      &pdf);

//            pdf /= numLights;
//            if(pdf == 0.f) {
//                L += Color3f(0.f);
//            } else {
//                // Solve for f
//                // just use f cause you don't want to sample anything. you just want a color.
//                f = intersection.bsdf->f(wo, glm::normalize(wi), type);

//                // Test for shadows
//                Intersection isx = Intersection();
//                Ray shadowFeeler = intersection.SpawnRay(wi);
//                bool shadowIntersects = scene.Intersect(shadowFeeler, &isx);

//                if(shadowIntersects &&
//                   (isx.objectHit->areaLight != scene.lights.at(lightPos)))
//                {
//                    L += Color3f(0.f);
//                }
//                else
//                {
//                    // This might be funky
//                    L += (f * li * AbsDot(wi, intersection.normalGeometric)) / pdf;
//                }

//                // This might be funky
//                L += beta * li;
//            }

//        }

//        //----------------------------------------------
//        // BSDF Sampling
//        //----------------------------------------------
//        Vector3f    wo = -r.direction;
//        Vector3f    wi;
//        float       pdf;
//        BxDFType    sampledType;
//        Color3f     f = intersection.bsdf->Sample_f(wo,
//                                                    &wi,
//                                                    sampler->Get2D(),
//                                                    &pdf,
//                                                    BSDF_ALL,
//                                                    &sampledType);

//        if(f == Color3f(0.f) || pdf == 0.f) {
//            break;
//        }

//        beta *= f * AbsDot(wi, intersection.normalGeometric) / pdf;

//        specularBounce = (BSDF_ALL & BSDF_SPECULAR) != 0;

//        r = intersection.SpawnRay(wi);

//        //----------------------------------------------
//        // Terminate with Russian Roulette
//        //----------------------------------------------
//        float prob = sampler->Get1D();
//        if(russianRoulette(compoundEnergy, prob, depth)) {
//            float q = std::max(.05f, 1 - beta.y);

//            if(prob < q) {
//                break;
//            }

//            beta /= 1.f - q;
//        }

//        depth--;
//    }


//    while(depth > 0)
//    {

//        //-----------------------------------------------
//        // Get intersection
//        //-----------------------------------------------

//        // Main intersection
//        Intersection intersection = Intersection();
//        // Sets intersection point, normal, etc.
//        bool intersects = scene.Intersect(ray, &intersection);

//        //-----------------------------------------------
//        // Direct Lighting
//        // TO MODIFY
//        //-----------------------------------------------

//        Vector3f    wo = -ray.direction;
//        Vector3f    wi;
//        float       pdf;
//        BxDFType    type = BSDF_ALL;
//        BxDFType    sampledType;

//        // Check to see if intersection actually hits anything
//        if(!intersects) {
//            //return Color3f(0.f);
//            //continue;
//        }

//        // If the ray doesn't hit a light, the contribution is black
//        if(intersection.objectHit->material != nullptr) {
//            L += Color3f(0.f);

//            // maybe continue since we should not
//            // include any other kind of global illumination
//        }
//        else {
//            L += intersection.Le(wo);
//        }

//        bool b = intersection.ProduceBSDF();
//        if(!b) {
//            ray = intersection.SpawnRay(ray.direction);
//            depth--;
//            continue;
//        }

//        //-----------------------------------------------
//        // Direct Lighting
//        // TO MODIFY
//        //-----------------------------------------------

//        Color3f f(0.f);

//        int lightPos = -1;
//        int numLights = scene.lights.size();

//        if(numLights == 0) {
//            L += Color3f(0.f);
//        } else {
//            // Get the light position using the sampler
//            lightPos = std::min((int)std::floor(sampler->Get1D() * numLights),
//                                               numLights - 1);

//            // Solve for li and set lighWi and lightPdf with Sample_Li rather
//            // than using Sample_f.
//            Color3f li = scene.lights.at(lightPos)->Sample_Li(intersection,
//                                                              sampler->Get2D(),
//                                                              &wi,
//                                                              &pdf);

//            pdf /= numlights;
//            if(pdf == 0.f) {
////                L += Color
//            }
//        }


//        float prob = 0.f;
//        if(russianRoulette(compoundEnergy, prob, depth)) {
//            break;
//        }

//        depth--;
//    }

//    return L;
//}


// This is mathematically incorrect. We need to take into account
// multiple light sources. The weighting is different. You will
// get a brighter and noiser result if you use this version of
// global illumination.
Color3f FullLightingIntegrator::LiWRONG(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundEnergy) const
{
    //-----------------------------------------------
    // Get intersection
    //-----------------------------------------------

    // Main intersection
    Intersection intersection = Intersection();
    // Sets intersection point, normal, etc.
    bool intersects = scene.Intersect(ray, &intersection);

    Vector3f wo = -ray.direction;
    // wi and pdf to be set in their respective functions
    BxDFType type = BSDF_ALL;
    BxDFType sampledType;

    // Check to see intersection actually hits anything
    if(!intersects) {
        return Color3f(0.f);
    }

    // Check to see if it hits a light or object
    if(intersection.objectHit->material == nullptr) {
//        if(depth == recursionLimit) {
//            return intersection.Le(wo);
//        } else {
//            return Color3f(0.f);
//        }

        return intersection.Le(wo);
    }

    bool b = intersection.ProduceBSDF();

    //-----------------------------------------------
    // Do Direct Lighting Integrator
    //-----------------------------------------------

    Vector3f lighWi;
    float lightPdf;

    Color3f lightF(0.f);
    // Final energy from direct lighting
    Color3f lightLte(0.f);

    // Set to -1 to identify if a light has been selected
    int lightPos = -1;

    // Pick a random light
    int numLights = scene.lights.size();

    if(numLights == 0) {
        lightLte = Color3f(0.f);
    }
    else {
        // Get the light position using the sampler
        lightPos = std::min((int)std::floor(sampler->Get1D() * numLights),
                                           numLights - 1);

        // Solve for li and set lighWi and lightPdf with Sample_Li rather
        // than using Sample_f.
        Color3f directLi = scene.lights.at(lightPos)->Sample_Li(intersection,
                                                                sampler->Get2D(),
                                                                &lighWi,
                                                                &lightPdf);

        lightPdf /= numLights;
        if(lightPdf == 0.f) {
            lightLte = Color3f(0.f);
        } else {
            // Solve for f
            // just use f cause you don't want to sample anything. you just want a color.
            lightF = intersection.bsdf->f(wo, glm::normalize(lighWi), type);

            // Test for shadows
            Intersection isx = Intersection();
            Ray shadowFeeler = intersection.SpawnRay(lighWi);
            bool shadowIntersects = scene.Intersect(shadowFeeler, &isx);

            if(shadowIntersects &&
               (isx.objectHit->areaLight != scene.lights.at(lightPos)))
            {
                lightLte = Color3f(0.f);
            }
            else
            {
                lightLte = (lightF * directLi * AbsDot(lighWi, intersection.normalGeometric)) / lightPdf;
            }
        }
    }

    //-----------------------------------------------
    // Do Naive Integrator
    //-----------------------------------------------

    Vector3f brdfWi;
    float brdfPdf;

    Color3f brdfF(0.f);
    // Final energy from BRDF
    Color3f brdfLte(0.f);

    float prob = sampler->Get1D();

    if(depth == 0 || russianRoulette(compoundEnergy, prob, depth)) {
        brdfLte = Color3f(0.f);
    } else {
        brdfF += intersection.bsdf->Sample_f(wo,
                                             &brdfWi,
                                             sampler->Get2D(),
                                             &brdfPdf,
                                             type,
                                             &sampledType);

        Ray r = intersection.SpawnRay(brdfWi);

        compoundEnergy = (compoundEnergy * brdfF * AbsDot(brdfWi, intersection.normalGeometric) / brdfPdf) / (1 - prob);

        if(brdfPdf == 0.f) {
            brdfLte = Color3f(0.f);
        } else {
            brdfLte = (brdfF * Li(r, scene, sampler, --depth, compoundEnergy) *
                       AbsDot(brdfWi, intersection.normalGeometric)) / brdfPdf;
        }
    }

    //-----------------------------------------------
    // Combine direct and naive to get full
    //-----------------------------------------------

    float brdfWeight = 1.f;
    float directWeight = 0.f;

    if(lightPos != -1) {
        brdfWeight = PowerHeuristic(1,
                                    brdfPdf,
                                    1,
                                    scene.lights.at(lightPos)->Pdf_Li(intersection, brdfWi) / scene.lights.length());

        directWeight = PowerHeuristic(1,
                                     lightPdf,
                                     1,
                                     intersection.bsdf->Pdf(wo, lighWi, type));
    }


    Color3f full = brdfWeight * brdfLte + directWeight * lightLte;

    return intersection.Le(wo) + full;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    float denom = nf * fPdf + ng * gPdf;

    if(fequal(denom, 0.f)) {
        return 0.f;
    }

    return (nf * fPdf) / denom;
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    float f = nf * fPdf;
    float g = ng * gPdf;
    float denom = f * f + g * g;

    if(fequal(denom, 0.f)) {
        return 0.f;
    }

    return (f * f) / denom;
}

bool FullLightingIntegrator::russianRoulette(Color3f &compoundEnergy, float prob, int depth) const
{
    // Make sure that at least three bounces
    if(depth <= (recursionLimit - 3)) {
        float energy = std::max(std::max(compoundEnergy.x, compoundEnergy.y) , compoundEnergy.z);

        if(energy < prob) {
            return true;
        }
    }

    return false;
}
