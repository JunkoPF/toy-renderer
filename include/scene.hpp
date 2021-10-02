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

    void AddObject(const shared_ptr<Object> &object_ptr) {
        list_.objects_.emplace_back(object_ptr);
        initialized_ = false;
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

private:
    void InitializeBvh() {
        bvh_tree_ = Bvh::BvhTree(list_);
        initialized_ = true;
    }

    ObjectList list_;
    Bvh::BvhTree bvh_tree_;
    Camera camera_;
    bool initialized_;
};

#endif