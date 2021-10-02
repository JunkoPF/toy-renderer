#ifndef MATERIAL_H
#define MATERIAL_H

#include "base.hpp"
#include "object.hpp"

struct hit_record;

class Material {
public:
    virtual bool scatter(const Ray &r_in, const Intersection &rec,
                         Color3d &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
    Lambertian(const Color3d &a) : albedo_(a) {}

    virtual bool scatter(const Ray &r_in, const Intersection &rec,
                         Color3d &attenuation, Ray &scattered) const override {
        auto scatter_direction = TrRandom::UnitVec3InHemisphere(rec.normal_);
        scattered = Ray(rec.p_, scatter_direction);
        attenuation = albedo_;
        return true;
    }

public:
    Color3d albedo_;
};

class Metal : public Material {
public:
    Metal(const Color3d &a, const double &f) : albedo(a), fuzz(fmin(f, 1.0)) {}

    virtual bool scatter(const Ray &r_in, const Intersection &rec,
                         Color3d &attenuation, Ray &scattered) const override {
        Vector3d scatter_direction = reflect(r_in.direction(), rec.normal_);
        if (fuzz > eps) {
            scatter_direction += fuzz * TrRandom::UnitVec3d();
        }
        scattered = Ray(rec.p_, scatter_direction);
        attenuation = albedo;
        return DotProduct(scattered.direction(), rec.normal_) > 0;
    }

public:
    Color3d albedo;
    double fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(double refractive_index) : ir(refractive_index) {}

    virtual bool scatter(const Ray &r_in, const Intersection &rec,
                         Color3d &attenuation, Ray &scattered) const override {
        attenuation = {1.0, 1.0, 1.0};
        double ratio = rec.front_face_ ? 1.0 / ir : ir;

        double cos_theta = fmin(DotProduct(-r_in.direction(), rec.normal_), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        Vector3d in_dir = Normalize(r_in.direction());
        Vector3d scatter_direction;

        if (ratio * sin_theta > 1.0 || reflectance(cos_theta, ratio) > TrRandom::Double()) {
            scatter_direction = reflect(in_dir, rec.normal_);
        } else {
            scatter_direction = refract(in_dir, rec.normal_, ratio);
        }
        scattered = Ray(rec.p_, scatter_direction);

        return true;
    }

public:
    double ir;

private:
    static double reflectance(double cosine, double ref_idx) {
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 *= r0;
        return r0 + (1 - r0) * pow(1 - cosine, 5);
    }
};
#endif