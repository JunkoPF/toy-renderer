#ifndef TR_INCLUDE_TRAINGLE_H
#define TR_INCLUDE_TRAINGLE_H

#include <array>

#include "object.hpp"

class Traingle : public Object {
public:
    virtual Intersection Intersect(const Ray &r, double t_min, double t_max) const override;
    virtual BoundingBox GetBoundingBox() const override;

private:
    std::array<Point3d, 3> vertex_;
};

Intersection Traingle::Intersect(const Ray &r, double t_min, double t_max) const { 
}

#endif