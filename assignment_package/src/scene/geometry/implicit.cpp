#include "implicit.h"

Bounds3f Implicit::WorldBound() const
{
    Point3f max(5.f,5.f,5.f);
    Point3f min(-5.f,-5.f,-5.f);
    Bounds3f bounds(min,max);

    return bounds.Apply(this->transform);
}

float Implicit::Area() const
{
    return 0;
}

float sceneSDF(Point3f p)
{
    float x = p.x;
    float y = p.y;
    float z = p.z;

    float x2 = x * x;
    float x4 = x2 * x2;
    float y2 = y * y;
    float y3 = y2 * y;
    float y4 = y2 * y2;
    float z2 = z * z;
    float z4 = z2 * z2;
    float z5 = z4 * z;


    // Chair
    float k = 5.f;
    float a = .95f;
    float b = .8f;

    float first = x2 + y2 + z2 - (a * k * k);
    float first2 = first * first;
    float second = (pow(z - k, 2) - 2 * x2) * (pow(z + k, 2) - 2 * y2);
    float bsecond = b * second;
    return first2 - bsecond;


//    return x4 + y4 + z4 - (x2 + y2 + z2);

//    return x2 + y3 + z5 - 1;

//     4(x^2+y^2+z^2-13)^3+27(3x^2+y^2-4z^2-12)^2=0.
//    return 4 * pow(x2 + y2 + z2- 13, 3) + 27 * pow(3 * x2 + y2 - 4 * z2 - 12, 2);

//     Tanglecube
    return x4 - (5 * x2) + y4 - (5 * y2) + z4 - (5 * z2) + 11.8;
}

glm::vec2 Implicit::GetUVCoordinates(const glm::vec3 &) const
{
   return glm::vec2(0.f,0.f);
}

bool Implicit::Intersect(const Ray& r, Intersection* isect) const
{
    //Transform the ray
    float t = 0.f;
    float stepSize = 0.1f;
    float smallStepSize = 0.02f;
    Ray r_loc = r.GetTransformedCopy(transform.invT());

    for (int i = 0; i < 500; i++) {

        Point3f p = r_loc.origin + r_loc.direction * t;
        float dist = sceneSDF(p);

        if (dist < 0.001) {
            t-=stepSize;
            for (int i = 0; i < 10; i++) {

                p = r_loc.origin + r_loc.direction * t;
                dist = sceneSDF(p);

                if (dist < 0.001) {
                    t -= smallStepSize;
                    p = r_loc.origin + r_loc.direction * t;
                    InitializeIntersection(isect, t, p);
                    return true;
                }

                t+= smallStepSize;
            }

            return false;
        }

        t+= stepSize;
    }

    return false;
}

Vector3f estimateNormal(Vector3f pos) {

    float epsilon = 0.0001f;
    Vector3f normal = Vector3f(0.f);

    normal.x = sceneSDF(Point3f(pos.x + epsilon, pos.y, pos.z))
              - sceneSDF(Point3f(pos.x - epsilon, pos.y, pos.z));
    normal.y = sceneSDF(Point3f(pos.x, pos.y + epsilon, pos.z))
              - sceneSDF(Point3f(pos.x, pos.y - epsilon, pos.z));
    normal.z = sceneSDF(Point3f(pos.x, pos.y, pos.z + epsilon))
              - sceneSDF(Point3f(pos.x, pos.y, pos.z - epsilon));

    return glm::normalize(normal);
}

void Implicit::ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const
{
    *nor = glm::normalize(transform.invTransT() * estimateNormal(P));

    *tan = glm::normalize(transform.T3() * glm::cross(Vector3f(0,1,0),estimateNormal(P)));
    *bit = glm::normalize(glm::cross(*nor, *tan));
}

Intersection Implicit::Sample(const Point2f &xi, Float *pdf) const {

    return Intersection();
}
