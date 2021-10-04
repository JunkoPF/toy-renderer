#ifndef HITTABLE_H
#define HITTABLE_H

#include "base.hpp"
#include "bounding_box.hpp"
#include "material.hpp"

class Material;

class Intersection {
public:
    Intersection() : happened_(false), t_(std::numeric_limits<double>::max()) {}

    inline void SetFaceNormal(const Ray &r, const Vector3d &outward_normal) {
        front_face_ = DotProduct(r.direction(), outward_normal) < 0;
        normal_ = front_face_ ? outward_normal : -outward_normal;
    }

    bool happened_;
    Point3d p_;
    Vector3d normal_;
    shared_ptr<Material> material_;
    double t_;
    bool front_face_;
};

class Object {
public:
    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const = 0;
    virtual BoundingBox GetBoundingBox() const = 0;
    virtual double GetArea() const = 0;
    virtual void Sample(Intersection &inter, double &pdf) const = 0;
    virtual MaterialPtrType GetMaterial() const = 0;
};

using ObjectPtrType = std::shared_ptr<Object>;
using ObjectListType = std::vector<ObjectPtrType>;

#endif