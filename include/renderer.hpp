#ifndef TR_INCLUDE_RENDERER_H
#define TR_INCLUDE_RENDERER_H

#include <mutex>
#include <thread>

#include "base.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "scene.hpp"

std::mutex mutex_ins;

const double russian_roulette = 0.8;

class Renderer {
public:
    Renderer() {}
    Renderer(int width, double aspect_ratio, int samples, int depth)
        : image_width_(width), image_height_(static_cast<int>(width / aspect_ratio)), samples_per_pixel_(samples), max_depth_(depth) {}

    /*
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
*/

    void Render(std::ostream &os, Scene &scene) const {
        std::vector<Color3d> frame_buffer(image_width_ * image_height_);

        int cnt = 0;

        auto get_color = [&](int row_begin, int row_end, int column_begin, int column_end) {
            for (int x = row_begin; x < row_end; ++x) {
                for (int y = column_begin; y < column_end; ++y) {
                    Color3d pixel_color(0, 0, 0);
                    for (int s = 0; s < samples_per_pixel_; ++s) {
                        auto u = (y + TrRandom::Double()) / (image_width_ - 1);
                        auto v = (x + TrRandom::Double()) / (image_height_ - 1);
                        Ray r = scene.camera_.GetRay(u, v);
                        pixel_color += CastRay(r, scene, max_depth_);
                    }
                    frame_buffer[x * image_width_ + y] = pixel_color;
                }
            }

            std::lock_guard<std::mutex> g1(mutex_ins);
            std::cerr << "\rFinish threads num: " << ++cnt << std::flush;
        };

        int block_width = 25, block_height = 25;
        int block_size_x = image_height_ / block_height;
        int block_size_y = image_width_ / block_width;

        std::vector<std::thread> threads;

        for (int x = 0; x < block_height; ++x) {
            for (int y = 0; y < block_width; ++y) {
                int x_begin = x * block_size_x;
                int y_begin = y * block_size_y;
                threads.emplace_back(get_color, x_begin, x_begin + block_size_x, y_begin, y_begin + block_size_y);
            }
        }

        for (auto &th : threads) {
            th.join();
        }

        os << "P3\n"
           << image_width_ << " " << image_height_ << "\n255\n";
        for (int j = image_height_ - 1; j >= 0; --j) {
            for (int i = 0; i < image_width_; ++i) {
                WriteColor(os, frame_buffer[j * image_width_ + i], samples_per_pixel_);
            }
        }
    }

    Color3d CastRay(const Ray &r, Scene &scene, int depth) const {
        Intersection inter;

        inter = scene.bvh_tree_.CheckIntersect(r, 0.001, infinity);

        if (!inter.happened_) {
            return {0, 0, 0};
        }

        // if hit light direction
        if (inter.material_->HasEmission()) {
            return inter.material_->GetEmission();
        }

        Vector3d L_dir{0, 0, 0}, L_indir{0, 0, 0};

        // TO DO: sample from light
        Intersection inter_light;
        double pdf_light = 0.0;
        scene.SampleLight(inter_light, pdf_light);

        Vector3d p = inter.p_;
        Vector3d N = inter.normal_;
        Vector3d w_o = -r.direction();

        Vector3d x = inter_light.p_;
        Vector3d NN = inter_light.normal_;

        Ray ligth_sample(p, Normalize(x - p));
        Intersection light_inter = scene.bvh_tree_.CheckIntersect(ligth_sample, 0.001, infinity);
        if (light_inter.happened_ && Length(light_inter.p_ - x) < eps && light_inter.material_->HasEmission()) {
            Vector3d w_s = Normalize(x - p);
            Vector3d fr = inter.material_->Eval(w_o, w_s, N);
            double dis = LengthSquared(x - p);
            double cos_theta_1 = DotProduct(w_s, N);
            double cos_theta_2 = DotProduct(-w_s, NN);
            if (cos_theta_1 * cos_theta_2 >= 0) {
                L_dir = HadamardProduct(light_inter.material_->GetEmission(), fr) * cos_theta_1 * cos_theta_2 / dis / pdf_light;
            }
            assert(L_dir.x() >= 0 && L_dir.y() >= 0 && L_dir.z() >= 0);
        }

        // Russian Roulette
        if (TrRandom::Double() <= russian_roulette) {
            Vector3d w_i = inter.material_->Sample(w_o, N);
            Ray next_ray(p, w_i);
            Intersection next_inter = scene.bvh_tree_.CheckIntersect(next_ray, 0.001, infinity);
            if (next_inter.happened_ && !next_inter.material_->HasEmission()) {
                double pdf = inter.material_->Pdf(w_i, w_o, N) + eps;
                if (pdf > eps) {
                    Vector3d fr = inter.material_->Eval(w_i, w_o, N);
                    L_indir = HadamardProduct(CastRay(next_ray, scene, depth + 1), fr) * DotProduct(w_i, N) / pdf / russian_roulette;
                    assert(L_indir.x() >= 0 && L_indir.y() >= 0 && L_indir.z() >= 0);
                }
            }
        }
        return L_dir + L_indir;
    }

private:
    int image_width_;
    int image_height_;
    int samples_per_pixel_;
    int max_depth_;
};

#endif