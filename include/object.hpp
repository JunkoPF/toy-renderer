#ifndef HITTABLE_H
#define HITTABLE_H

#include "base.hpp"
#include "bounding_box.hpp"

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
};

#endif