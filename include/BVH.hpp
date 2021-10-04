#ifndef TR_INCLUDE_BVH_H
#define TR_INCLUDE_BVH_H

#include <algorithm>
#include <queue>

#include "base.hpp"
#include "object.hpp"
#include "object_list.hpp"

namespace Bvh {
class BvhNode;

using NodePtrType = shared_ptr<BvhNode>;

class BvhNode {

public:
    BvhNode();

    BvhNode(const ObjectListType &list, size_t begin_index, size_t end_index);

    BvhNode(const ObjectListType &list) : BvhNode(list, 0, list.size()) {}

    const BoundingBox &GetBox() const { return box_; }
    bool IsLeaf() const { return object_ != nullptr; }

public:
    NodePtrType left_;
    NodePtrType right_;
    ObjectPtrType object_;
    BoundingBox box_;
};

BvhNode::BvhNode(const ObjectListType &objects, size_t begin_index, size_t end_index) {
    ObjectListType tmp_objects(objects);
    size_t num_objects = end_index - begin_index;
    assert(num_objects > 0);

    if (num_objects == 1) {
        object_ = objects[begin_index];
    } else {
        BoundingBox centroid_box(objects[begin_index]->GetBoundingBox().Centroid());
        for (int i = begin_index + 1; i < end_index; ++i) {
            centroid_box = MergeBoxes(centroid_box, BoundingBox(objects[i]->GetBoundingBox().Centroid()));
        }

        size_t axis = MaxDim(centroid_box);

        std::sort(
        tmp_objects.begin() + begin_index, tmp_objects.begin() + end_index,
        [&axis](ObjectPtrType &a, ObjectPtrType &b) {
            return BoxesCentroidCompare(a->GetBoundingBox(), b->GetBoundingBox(), axis);
        });
        size_t mid = begin_index + num_objects / 2;
        left_ = make_shared<BvhNode>(objects, begin_index, mid);
        right_ = make_shared<BvhNode>(objects, mid, end_index);
    }

    if (num_objects == 1) {
        box_ = object_->GetBoundingBox();
    } else {
        box_ = MergeBoxes(left_->box_, right_->box_);
    }
}

class BvhTree {
public:
    BvhTree(){};

    BvhTree(const ObjectListType &objects) {
        root_ = make_shared<BvhNode>(objects);
    };

    Intersection CheckIntersect(const Ray &r, double t_min, double t_max) const;

public:
    NodePtrType root_;
};

// use stack instead of recursion
Intersection BvhTree::CheckIntersect(const Ray &r, double t_min, double t_max) const {
    std::queue<NodePtrType> node_queue;
    node_queue.push(root_);
    Intersection ret_intersection;

    while (!node_queue.empty()) {
        NodePtrType cur_node = node_queue.front();
        node_queue.pop();
        if (cur_node->GetBox().Check(r, t_min, t_max)) {
            if (cur_node->object_) {
                Intersection cur_intersection = cur_node->object_->Intersect(r, t_min, t_max);
                if (cur_intersection.happened_ && cur_intersection.t_ < ret_intersection.t_) {
                    ret_intersection = cur_intersection;
                }
            } else {
                if (cur_node->left_->GetBox().Check(r, t_min, t_max)) {
                    node_queue.push(cur_node->left_);
                }
                if (cur_node->right_->GetBox().Check(r, t_min, t_max)) {
                    node_queue.push(cur_node->right_);
                }
            }
        }
    }
    return ret_intersection;
}
} // namespace Bvh
#endif