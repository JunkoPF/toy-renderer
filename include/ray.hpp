#ifndef RAY_H
#define RAY_H

#include "base.hpp"

using TrMatrix::Base::Vector3d;
using TrMatrix::Util::Normalize;

class Ray {
public:
    Ray() {}
    Ray(const Point3d &origin, const Vector3d &direction)
        : origin_(origin), direction_(Normalize(direction)) {}

    const Point3d &origin() const { return origin_; }
    const Vector3d &direction() const { return direction_; }

    Point3d at(double t) const {
        return origin_ + direction_ * t;
    }

private:
    Point3d origin_;
    Vector3d direction_;
};

#endif