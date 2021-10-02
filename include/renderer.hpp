#ifndef TR_INCLUDE_RENDERER_H
#define TR_INCLUDE_RENDERER_H

#include "BVH.hpp"
#include "base.hpp"
#include "camera.hpp"
#include "material.hpp"

class Renderer {
public:
    Renderer() {}
    Renderer(int width, double aspect_ratio, int samples, int depth)
        : image_width_(width), image_height_(static_cast<int>(width / aspect_ratio)), samples_per_pixel_(samples), max_depth_(depth) {}

    void Render(std::ostream &os, const Camera &cam, const Bvh::BvhTree bvh_tree) const {
        os << "P3\n"
           << image_width_ << " " << image_height_ << "\n255\n";

        for (int j = image_height_ - 1; j >= 0; --j) {
            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width_; ++i) {
                Color3d pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel_; ++s) {
                    auto u = (i + TrRandom::Double()) / (image_width_ - 1);
                    auto v = (j + TrRandom::Double()) / (image_height_ - 1);
                    Ray r = cam.GetRay(u, v);
                    pixel_color += CastRay(r, bvh_tree, max_depth_);
                }
                WriteColor(os, pixel_color, samples_per_pixel_);
            }
        }
    }

private:
    int image_width_;
    int image_height_;
    int samples_per_pixel_;
    int max_depth_;

    Color3d CastRay(const Ray &r, const Bvh::BvhTree &bvh, int depth) const {
        Intersection rec;

        if (depth <= 0) {
            return {0, 0, 0};
        }

        rec = bvh.CheckIntersect(r, 0.001, infinity);

        if (rec.happened_) {
            Ray scattered;
            Color3d attenuation;
            if (rec.material_->scatter(r, rec, attenuation, scattered)) {
                Color3d ret = HadamardProduct(attenuation, CastRay(scattered, bvh, depth - 1));
                return ret;
            }
            return Color3d(0, 0, 0);
        }
        Vector3d dir = Normalize(r.direction());
        auto t = 0.5 * (dir.y() + 1.0);
        return (1.0 - t) * Color3d(1.0, 1.0, 1.0) + t * Color3d(0.5, 0.7, 1.0);
    }
};

#endif