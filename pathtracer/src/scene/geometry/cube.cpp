#include "cube.h"
#include <iostream>

float Cube::Area() const
{
    //TODO
    return 0;
}

// Returns +/- [0, 2]
int GetFaceIndex(const Point3f& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i * glm::sign(P[i]);
            val = glm::abs(P[i]);
        }
    }
    return idx;
}

Normal3f GetCubeNormal(const Point3f& P)
{
    int idx = glm::abs(GetFaceIndex(Point3f(P)));
    Normal3f N(0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}


bool Cube::Intersect(const Ray& r, Intersection* isect) const
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return false;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    if(t_n < t_f)
    {
        float t = t_n > 0 ? t_n : t_f;

        // all intersection tests should return 0 if t is negative
        if(t < 0)
            return false;
        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t*r_loc.direction, 1);
        InitializeIntersection(isect, t, Point3f(P));
        return true;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return false;
    }
}


void Cube::ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const
{
    *nor = glm::normalize(transform.invTransT() * GetCubeNormal(P));
    //TODO: Compute tangent and bitangent

    Normal3f n = GetCubeNormal(P);

    // http://www.gamasutra.com/blogs/RobertBasler/20131122/205462/Three_Normal_Mapping_Techniques_Explained_For_the_Mathematically_Uninclined.php?print=1
    if(n == Point3f(1,0,0)) {
        *tan = glm::normalize(transform.T3() * Normal3f(0,0,-1));
    } else if(n == Point3f(-1,0,0)) {
        *tan = glm::normalize(transform.T3() * Normal3f(0,0,1));
    } else if(n == Point3f(0,1,0)) {
        *tan = glm::normalize(transform.T3() * Normal3f(0,0,1));
    } else if(n == Point3f(0,-1,0)) {
        *tan = glm::normalize(transform.T3() * Normal3f(0,0,-1));
    } else if(n == Point3f(0,0,1)) {
        *tan = glm::normalize(transform.T3() * Normal3f(1,0,0));
    } else if(n == Point3f(0,0,-1)) {
        *tan = glm::normalize(transform.T3() * Normal3f(-1,0,0));
    }

    *bit = glm::normalize(glm::cross(*nor, *tan));
}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point) const
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}

Intersection Cube::Sample(const Point2f &xi, Float *pdf) const
{
    Point3f p(xi.x - 0.5f, xi.y - 0.5f, 0.f);

    Intersection intersection = Intersection();

    intersection.normalGeometric = glm::normalize(this->transform.invTransT() * Normal3f(0,0,1));

    intersection.point = glm::vec3(this->transform.T() * glm::vec4(p, 1.f));

    return intersection;
}

Bounds3f Cube::WorldBound() const
{
    Point3f min(-.5f, -.5, -.5f);
    Point3f max(.5f, .5, .5f);

    Bounds3f bounds(min, max);
    bounds = bounds.Apply(transform);

    return bounds;
}
