#ifndef TR_INCLUDE_TRAINGLE_H
#define TR_INCLUDE_TRAINGLE_H

#include <array>

#include "object.hpp"

class Traingle : public Object {
public:
    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;

private:
    std::array<Point3d, 3> vertex_coords_;
    std::array<Vector3d, 2> edges_;
    std::array<Point3d, 3> texture_coords_;
    Vector3d normal_;
};

Intersection Traingle::Intersect(const Ray &r, double t_min, double t_max) const {
    // using Moller Trumbore Algorithm to get 
    Intersection ret_intersection;
    if (DotProduct(ray.direction(), normal_) > 0) return inter;
    double u, v, t_tmp = 0;
    Vector3d pvec = CrossProduct(ray.direction(), edges_[1]);
    double det = DotProduct(edges_[0], pvec);
    if (fabs(det) < eps) return inter;

    double det_inv = 1.0 / det;
    Vector3d tvec = ray.origin() - vertex_coords_[0];
    u = DotProduct(tvec, pvec) * det_inv;
    if (u < 0 || u > 1) return inter;
    Vector3d qvec = CrossProduct(tvec, edges_[0]);
    v = DotProduct(ray.direction(), qvec) * det_inv;
    if (v < 0 || u + v > 1) return inter;
    t_tmp = DotProduct(edges_[1], qvec) * det_inv;
    if (t_tmp < 0) return inter;
    // TODO find ray triangle intersection

    inter.happened = true;
    inter.coords = r(t_tmp);
    inter.distance = t_tmp;
    inter.normal = this->normal;
    inter.obj = this;
    inter.m = this->m;

    return inter;
}

#endif