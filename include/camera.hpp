#ifndef TR_INCLUDE_CAMERA_H
#define TR_INCLUDE_CAMERA_H

#include "base.hpp"

class Camera {
public:
    Camera() {}

    Camera(Point3d view_pos,
           Point3d look_at_pos,
           Vector3d up_dir,
           double fov = 60.0,
           double aspect_ratio = 16.0 / 9.0,
           double focus_dist = 1.0,
           double aperture = 1.0)
        : view_pos_(view_pos),
          look_at_pos_(look_at_pos),
          up_dir_(up_dir),
          aspect_ratio_(aspect_ratio),
          lens_radius_(aperture / 2.0) {

        double h = tan(DegreesToRadians(fov) / 2);
        double viewport_height = h * 2.0;
        double viewport_width = viewport_height * aspect_ratio;

        Vector3d look_dir = Normalize(look_at_pos_ - view_pos_);

        w_ = look_dir;
        u_ = Normalize(CrossProduct(w_, up_dir));
        v_ = CrossProduct(u_, w_);

        horizontal_ = u_ * viewport_width * focus_dist;
        vertical_ = v_ * viewport_height * focus_dist;

        lower_left_corner_ = view_pos - horizontal_ / 2 - vertical_ / 2 + focus_dist * w_;
    }

    Ray GetRay(double s, double t) const {
        Vector3d rd = lens_radius_ * TrRandom::Vec3InUnitDisk();
        Vector3d offset = rd.x() * u_ + rd.y() * v_;

        return Ray(view_pos_ + offset, lower_left_corner_ + s * horizontal_ + t * vertical_ - view_pos_ - offset);
    }

private:
    double aspect_ratio_;
    double viewport_height_;
    double focus_dist_;
    double lens_radius_;

    Point3d view_pos_;
    Vector3d look_at_pos_;
    Vector3d up_dir_;
    Vector3d horizontal_;
    Vector3d vertical_;
    Vector3d u_, v_, w_;
    Point3d lower_left_corner_;
};

#endif