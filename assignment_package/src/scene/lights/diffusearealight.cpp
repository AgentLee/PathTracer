#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO
    Color3f color(0.f);

    // Checks to see if the area light is only from one side of the shape's surface.

    // NEED TO CHECK IF ITS TWO SIDED
    // It's set for each object.
    // I was thinking how they do it in the book.
    if(glm::dot(isect.normalGeometric, w) > 0.f || twoSided) {
        return emittedLight;
    }

    return color;
}

Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wi, Float *pdf) const
{
    Color3f color(0.f);

///     Step 1. Get an intersection     ///
    Intersection intersection = Intersection();
    intersection = shape->Sample(ref, xi, pdf);     // tbd

///     Step 2. Check intersection and pdf      ///
    // If intersection and reference intersection are the same point,
    // return black

//    std::cout << intersection.point.x << " " << intersection.point.y << " " << intersection.point.z << std::endl;

    if(intersection.point == ref.point) {
        return color;
    }

    // If pdf is 0, return black
    if(*pdf == 0.f) {
        return color;
    }

///     Step 3. Set wi to normalized vector from reference point to isx point     ///
    *wi = glm::normalize(intersection.point - ref.point);

///     Step 4. Return light emitted along wi from isx      ///
    color = L(intersection, -*wi);

    return color;
}

// Returns the PDF of the light's shape given an intersection from which
// the light is being viewed and has a viewing direction wi.
float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    /// Step 1. Intersect sample ray with area light geometry
    Ray ray = ref.SpawnRay(wi);

    Intersection isectLight = Intersection();
    bool intersects = shape->Intersect(ray, &isectLight);

    if(!intersects) {
        return 0.f;
    }

    /// Step 2. Convert light sample weight to solid angle measure
    float pdf = glm::length2(ref.point - isectLight.point) /
            (AbsDot(isectLight.normalGeometric, -glm::normalize(wi)) * shape->Area());

    return pdf;
}
