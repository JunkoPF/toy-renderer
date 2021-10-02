#ifndef TR_INCLUDE_SPHERE_H
#define TR_INCLUDE_SPHERE_H

#include "base.hpp"
#include "bounding_box.hpp"
#include "material.hpp"
#include "object.hpp"

class Sphere : public Object {
public:
    Sphere() {}
    Sphere(Point3d cen, double r, shared_ptr<Material> m)
        : center_(cen), radius_(r), material_(m){};

    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;

public:
    Point3d center_;
    double radius_;
    shared_ptr<Material> material_;
};

Intersection Sphere::Intersect(const Ray &r, double t_min, double t_max) const {
    Vector3d oc = r.origin() - center_;
    double a = LengthSquared(r.direction());
    double hb = DotProduct(oc, r.direction());
    double c = DotProduct(oc, oc) - radius_ * radius_;

    double discriminant = hb * hb - a * c;

    Intersection ret_intersection;

    if (discriminant < 0) {
        return ret_intersection;
    }
    double root = (-hb - sqrt(discriminant)) / a;
    if (root < t_min || root > t_max) {
        root = (-hb + sqrt(discriminant)) / a;
        if (root < t_min || root > t_max) {
            return ret_intersection;
        }
    }

    ret_intersection.happened_ = true;
    ret_intersection.t_ = root;
    ret_intersection.p_ = r.at(root);
    Vector3d outward_normal_ = (ret_intersection.p_ - center_) / radius_;
    ret_intersection.SetFaceNormal(r, outward_normal_);
    ret_intersection.material_ = material_;

    return ret_intersection;
}

BoundingBox Sphere::GetBoundingBox() const {
    double r = fabs(radius_);
    return BoundingBox(center_ - Vector3d(r, r, r),
                       center_ + Vector3d(r, r, r));
}

#endif