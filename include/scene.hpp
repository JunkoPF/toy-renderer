#ifndef TR_INCLUDE_SCENE_H
#define TR_INCLUDE_SCENE_H

#include "BVH.hpp"
#include "base.hpp"

class Scene {

public:
    Scene() {}
    Scene(Camera cam) : camera_(cam), initialized_(false) {}

    void AddObject(const ObjectPtrType &object_ptr) {
        list_.emplace_back(object_ptr);
        initialized_ = false;
    }

    void AddObject(const ObjectListType &object_list) {
        list_.insert(list_.end(), object_list.begin(), object_list.end());
    }

    void SetCamera(const Camera &cam_) {
        camera_ = cam_;
    }

    void InitializeBvh() {
        bvh_tree_ = Bvh::BvhTree(list_);
        initialized_ = true;
    }

    Bvh::BvhTree GetBvhTree() {
        return bvh_tree_;
    }

    void SampleLight(Intersection &inter, double &pdf);

public:
    ObjectListType list_;
    Bvh::BvhTree bvh_tree_;
    Camera camera_;
    bool initialized_;
};

void Scene::SampleLight(Intersection &inter, double &pdf) {
    double sum_emit_area = 0.0;
    for (const auto &object : list_) {
        if (object->GetMaterial()->HasEmission()) {
            sum_emit_area += object->GetArea();
        }
    }
    double random_pick = TrRandom::Double() * sum_emit_area;
    for (const auto &object : list_) {
        if (object->GetMaterial()->HasEmission()) {
            if (random_pick < object->GetArea()) {
                object->Sample(inter, pdf);
                return;
            }
            random_pick -= object->GetArea();
        }
    }
}

#endif