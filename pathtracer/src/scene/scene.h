#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/lights/light.h>
#include <scene/geometry/shape.h>
#include <scene/medium.h>

#include "csg.h"
#include "bvh.h"
#include "kdtree.h"

class Primitive;
class BVHAccel;
class Material;
class Light;

class Scene
{
public:
    Scene();
    ~Scene();
    QList<std::shared_ptr<Primitive>> primitives;
    QList<std::shared_ptr<Material>> materials;
    QList<std::shared_ptr<Light>> lights;

    Camera camera;
    Film film;

    BVHAccel* bvh;
    KDTreeAccel *kdtree;

    bool hasMedium;
    bool makeCSG;
    CSG *csg;

    QList<std::shared_ptr<Drawable>> drawables;

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();

    bool Intersect(const Ray& ray, Intersection* isect) const;

    void clearBVH();
    void clearKDTree();
};
