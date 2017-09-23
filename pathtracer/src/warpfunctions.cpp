#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    float r = sqrt(sample.x);
    float theta = 2.f * Pi * sample.y;

    float x = r * cos(theta);
    float y = r * sin(theta);

    return Point3f(x, y, 0.f);
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    float phi = 0.f;

    float r = 0.f;
    float u = 0.f;
    float v = 0.f;

    float a = 2.f * sample.x - 1.f;
    float b = 2.f * sample.y - 1.f;

    if(a > -b) {
        if(a > b) {
            r = a;
            phi = (Pi / 4.f) * (b / a);
        } else {
            r = b;
            phi = (Pi / 4.f) * (2.f - (a / b));
        }
    } else {
        if(a < b) {
            r = -a;
            phi = (Pi / 4.f) * (4.f + (b / a));
        } else {
            r = -b;

            if(b != 0.f) {
                phi = (Pi / 4.f) * (6.f- (a / b));
            } else {
                phi = 0.f;
            }
        }
    }

    u = r * cos(phi);
    v = r * sin(phi);

    return Point3f(u, v, 0.f);
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    return InvPi;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    float z = 1.f - 2.f * sample.x;
    float r = sqrt(fmax(0.f, 1.f - pow(z, 2.f)));

    float phi = 2.f * Pi * sample.y;

    return Point3f(r * cos(phi), r * sin(phi), z);
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    return Inv4Pi;
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    float rad = (thetaMin) * Pi / 180.f;

    // 0 --> 2 (sphere)
    // 180 --> 1 (hemisphere)
    // sphere --> 1 - 2 * sample.x
    // 2 is some coefficient resulting from (180 - thetaMin)
    float z = 1.f - (2.f * (Pi - rad) /  Pi) * sample.x;

    float r = sqrt(fmax(0.f, 1.f - z * z));

    float theta = 2 * Pi * sample.y;

    return Point3f(r * cos(theta), r * sin(theta), z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    float rad = (180.f - thetaMin) * Pi / 180.f;

    return 1.f / (2.f * Pi * (1.f - cos(rad)));
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    float z = sample.x;
    float r = sqrt(fmax(0.f, 1.f - pow(z, 2.f)));

    float phi = 2.f * Pi * sample.y;

    return Point3f(r * cos(phi), r * sin(phi), z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    return Inv2Pi;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    Point3f s = squareToDiskConcentric(sample);

    float z = std::sqrt(std::fmax(0.f, 1.f - pow(s.x, 2.f) - pow(s.y, 2.f)));

    return Point3f(s.x, s.y, z);
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    float theta = acos(sample.z);

    return cos(theta) * Pi;
}
