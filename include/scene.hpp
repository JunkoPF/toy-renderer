#ifndef TR_INCLUDE_SCENE_H
#define TR_INCULDE_SCENE_H

#include "BVH.hpp"
#include "base.hpp"
#include "object_list.hpp"
#include "renderer.hpp"

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

    void Render(std::ostream &os, const Renderer &renderer) {
        if (!initialized_) {
            InitializeBvh();
        }
        renderer.Render(os, camera_, bvh_tree_);
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

public:
    ObjectListType list_;
    Bvh::BvhTree bvh_tree_;
    Camera camera_;
    bool initialized_;
};

#endif