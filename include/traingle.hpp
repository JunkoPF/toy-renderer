#ifndef TR_INCLUDE_TRIANGLE_H
#define TR_INCLUDE_TRIANGLE_H

#include <array>

#include "object.hpp"

class Triangle : public Object {
public:
    Triangle() {}
    Triangle(Vector3d v0, Vector3d v1, Vector3d v2, shared_ptr<Material> m)
        : vertex_coords_({v0, v1, v2}), material_(m) {
        edges_[0] = v1 - v0;
        edges_[1] = v2 - v0;
        normal_ = Normalize(CrossProduct(edges_[0], edges_[1]));
    }

    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;

private:
    std::array<Point3d, 3> vertex_coords_;
    std::array<Vector3d, 2> edges_;
    std::array<Point3d, 3> texture_coords_;
    shared_ptr<Material> material_;
    Vector3d normal_;
};

Intersection Triangle::Intersect(const Ray &r, double t_min, double t_max) const {
    // using Moller Trumbore Algorithm to get
    Intersection ret_intersection;
    if (DotProduct(r.direction(), normal_) > 0) return ret_intersection;
    double u, v, t_tmp = 0;
    Vector3d pvec = CrossProduct(r.direction(), edges_[1]);
    double det = DotProduct(edges_[0], pvec);
    if (fabs(det) < eps) return ret_intersection;

    double det_inv = 1.0 / det;
    Vector3d tvec = r.origin() - vertex_coords_[0];
    u = DotProduct(tvec, pvec) * det_inv;
    if (u < 0 || u > 1) return ret_intersection;
    Vector3d qvec = CrossProduct(tvec, edges_[0]);
    v = DotProduct(r.direction(), qvec) * det_inv;
    if (v < 0 || u + v > 1) return ret_intersection;
    t_tmp = DotProduct(edges_[1], qvec) * det_inv;
    if (t_tmp <= t_min || t_tmp >= t_max) return ret_intersection;
    // TODO find ray triangle intersection

    ret_intersection.happened_ = true;
    ret_intersection.p_ = r.at(t_tmp);
    ret_intersection.t_ = t_tmp;
    ret_intersection.normal_ = this->normal_;
    ret_intersection.material_ = material_;
    return ret_intersection;
}

BoundingBox Triangle::GetBoundingBox() const {
    return MergeBoxes(BoundingBox(vertex_coords_[0], vertex_coords_[1]), BoundingBox(vertex_coords_[2]));
}

#endif