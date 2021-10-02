#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include "object.hpp"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class ObjectList : public Object {
public:
    ObjectList() {}
    ObjectList(shared_ptr<Object> object) { add(object); }

    void clear() { objects_.clear(); }
    void add(shared_ptr<Object> object) { objects_.emplace_back(object); }

    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;

public:
    std::vector<shared_ptr<Object>> objects_;
};

Intersection ObjectList::Intersect(const Ray &r, double t_min, double t_max) const {
    Intersection ret_intersection;
    auto closest_so_far = t_max;

    for (const auto &object : objects_) {
        Intersection cur_intersection = object->Intersect(r, t_min, closest_so_far);
        if (cur_intersection.happened_) {
            closest_so_far = cur_intersection.t_;
            ret_intersection = cur_intersection;
        }
    }

    return ret_intersection;
}

BoundingBox ObjectList::GetBoundingBox() const {
    assert(!objects_.empty());
    BoundingBox box;
    for (const auto &object : objects_) {
        if (box.available()) {
            box = MergeBoxes(box, object->GetBoundingBox());
        } else {
            box = object->GetBoundingBox();
        }
    }
    return box;
}

#endif