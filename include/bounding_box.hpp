#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "base.hpp"

class BoundingBox {
public:
    BoundingBox() : min_(0, 0, 0), max_(0, 0, 0) {}
    BoundingBox(const Point3d &min, const Point3d &max) : min_(min), max_(max) {}

    const Point3d &min() const { return min_; }
    const Point3d &max() const { return max_; }
    const bool available() const { return min_ != max_; }

    bool Check(const Ray &r, double t_min, double t_max) const {
        Vector3d inv = Inverse(r.direction());
        for (int i = 0; i < 3; ++i) {
            double t0 = (min_(i) - r.origin()(i)) * inv(i);
            double t1 = (max_(i) - r.origin()(i)) * inv(i);
            if (t0 > t1) {
                std::swap(t0, t1);
            }
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min) {
                return false;
            }
        }
        return true;
    }

private:
    Point3d min_;
    Point3d max_;
};

BoundingBox MergeBoxes(const BoundingBox &lhs, const BoundingBox &rhs) {
    Point3d new_min, new_max;
    for (int i = 0; i < 3; ++i) {
        new_min(i) = fmin(lhs.min()(i), rhs.min()(i));
        new_max(i) = fmax(lhs.max()(i), rhs.max()(i));
    }
    return BoundingBox(new_min, new_max);
}

bool BoxesMinCompare(const BoundingBox &lhs, const BoundingBox &rhs, std::size_t axis) {
    assert(lhs.available() && rhs.available());
    return lhs.min()(axis) < rhs.min()(axis);
}

bool BoxesMaxCompare(const BoundingBox &lhs, const BoundingBox &rhs, std::size_t axis) {
    assert(lhs.available() && rhs.available());
    return lhs.max()(axis) < rhs.max()(axis);
}

bool BoxesCentroidCompare(const BoundingBox &lhs, const BoundingBox &rhs, std::size_t axis) {
    assert(lhs.available() && rhs.available());
    return lhs.min()(axis) + lhs.max()(axis) < rhs.min()(axis) + lhs.max()(axis);
}

std::size_t MaxDim(const BoundingBox &box) {
    double x = box.max()(0) - box.min()(0);
    double y = box.max()(1) - box.min()(1);
    double z = box.max()(2) - box.min()(2);
    if (x >= y && x >= z) {
        return 0;
    } else if (y >= x && y >= z) {
        return 1;
    }
    return 2;
}

#endif