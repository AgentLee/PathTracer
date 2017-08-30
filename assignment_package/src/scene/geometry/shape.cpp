#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());

void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

/// This invokes two-input Sample() which
/// is implemented by individual subclasses
/// to generate wi from the found intersection.
/// It must convert the PDF from the other Sample().
///
/// ref is the intersection from camera and scene
Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    // Don't call the shape's sample or area
    // This is because you're using ref which isn't the light.
    Intersection intersection = Sample(xi, pdf);

    Vector3f wi = glm::normalize(intersection.point - ref.point);

    if(AbsDot(intersection.normalGeometric, -wi) == 0.f) {
        *pdf = 0.f;
    } else {
        *pdf = glm::distance2(intersection.point, ref.point) / (AbsDot(intersection.normalGeometric, -wi) * Area());
    }

    return intersection;
}
