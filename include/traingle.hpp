#ifndef TR_INCLUDE_TRIANGLE_H
#define TR_INCLUDE_TRIANGLE_H

#include <array>
#include <string>

#include "BVH.hpp"
#include "OBJ_Loader.hpp"
#include "base.hpp"
#include "bounding_box.hpp"
#include "material.hpp"
#include "object.hpp"
#include "object_list.hpp"

class Triangle : public Object {
public:
    Triangle() {}
    Triangle(Vector3d v0, Vector3d v1, Vector3d v2, shared_ptr<Material> m)
        : vertex_coords_({v0, v1, v2}), material_(m) {
        edges_[0] = v1 - v0;
        edges_[1] = v2 - v0;

        Vector3d tmp = CrossProduct(edges_[0], edges_[1]);
        normal_ = Normalize(tmp);
        surface_area_ = 0.5 * Length(tmp);
    }

    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;
    virtual void Sample(Intersection &inter, double &pdf) const override;

    virtual double GetArea() const override { return surface_area_; }
    virtual MaterialPtrType GetMaterial() const override { return material_; }

private:
    std::array<Point3d, 3> vertex_coords_;
    std::array<Vector3d, 2> edges_;
    std::array<Point3d, 3> texture_coords_;
    shared_ptr<Material> material_;
    Vector3d normal_;

    double surface_area_;
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

void Triangle::Sample(Intersection &inter, double &pdf) const {
    double x = std::sqrt(TrRandom::Double()), y = TrRandom::Double();
    inter.p_ = vertex_coords_[0] * (1.0 - x) + vertex_coords_[1] * (x * (1.0 - y)) + vertex_coords_[2] * (x * y);
    inter.normal_ = normal_;
    pdf = 1.0 / surface_area_;
}

class MeshTriangle : public Object {
public:
    MeshTriangle(const objl::Mesh &mesh, shared_ptr<Material> material) : material_(material) {
        Vector3d min_vertex = Vector3d{infinity, infinity, infinity};
        Vector3d max_vertex = Vector3d{-infinity, -infinity, -infinity};

        surface_area_ = 0.0;

        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            std::array<Vector3d, 3> face_vertices;
            for (int j = 0; j < 3; ++j) {
                Vector3d vertex = Vector3d(mesh.Vertices[i + j].Position.X,
                                           mesh.Vertices[i + j].Position.Y,
                                           mesh.Vertices[i + j].Position.Z);
                face_vertices[j] = vertex;

                min_vertex = Vector3d(fmin(min_vertex.x(), vertex.x()), fmin(min_vertex.y(), vertex.y()), fmin(min_vertex.z(), vertex.z()));
                max_vertex = Vector3d(fmax(max_vertex.x(), vertex.x()), fmax(max_vertex.y(), vertex.y()), fmax(max_vertex.z(), vertex.z()));
            }

            auto triangle_ptr = make_shared<Triangle>(face_vertices[0], face_vertices[1], face_vertices[2], material);
            surface_area_ += triangle_ptr->GetArea();
            triangles_.emplace_back(triangle_ptr);
        }

        box_ = BoundingBox(min_vertex, max_vertex);

        bvh_tree_ = Bvh::BvhTree(triangles_);
    }

    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;
    virtual void Sample(Intersection &inter, double &pdf) const override;

    virtual double GetArea() const override { return surface_area_; }
    virtual MaterialPtrType GetMaterial() const override { return material_; }

public:
    ObjectListType triangles_;

    BoundingBox box_;

    Bvh::BvhTree bvh_tree_;

    double surface_area_;

    MaterialPtrType material_;
};

Intersection MeshTriangle::Intersect(const Ray &r, double t_min, double t_max) const {
    return bvh_tree_.CheckIntersect(r, t_min, t_max);
}

BoundingBox MeshTriangle::GetBoundingBox() const {
    return box_;
}

void MeshTriangle::Sample(Intersection &inter, double &pdf) const {
    //bvh_tree_.Sample(inter, pdf);
    double tmp_p = TrRandom::Double() * GetArea();
    for (const auto &tri : triangles_) {
        if (tri->GetArea() > tmp_p) {
            tri->Sample(inter, pdf);
            return;
        }
        tmp_p -= tri->GetArea();
    }
}

ObjectListType LoadObjectModel(std::string filename, shared_ptr<Material> material) {
    objl::Loader loader;
    loader.LoadFile(filename);
    auto meshes = loader.LoadedMeshes;

    ObjectListType mesh_list;

    for (auto &mesh : meshes) {
        mesh_list.emplace_back(make_shared<MeshTriangle>(mesh, material));
    }

    return mesh_list;
}

#endif