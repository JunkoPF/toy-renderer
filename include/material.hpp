#ifndef MATERIAL_H
#define MATERIAL_H

#include "base.hpp"
#include "object.hpp"

class Material {
public:
    enum MaterialType { kDIFFUSE,
                        kMICROFACET };

    Material() {}
    Material(MaterialType type, Vector3d kd, Vector3d ks, Vector3d ke, double ior, double roughness = 1.0, double metallic = 0.0)
        : type_(type), k_diffuse_(kd), k_specular_(ks), k_emission_(ke), index_of_refraction_(ior), roughness_(roughness), metallic_(metallic) {}

    bool HasEmission() const { return Length(k_emission_) > eps; }

    Color3d GetEmission() const { return k_emission_; }

    Vector3d Eval(const Vector3d &in_dir, const Vector3d &out_dir, const Vector3d &normal) const;

    Vector3d Sample(const Vector3d &in_dir, const Vector3d &normal) const;

    double Pdf(const Vector3d &in_dir, const Vector3d &out_dir, const Vector3d &normal) const;

private:
    MaterialType type_;
    Vector3d k_diffuse_;
    Vector3d k_specular_;
    Vector3d k_emission_;
    double index_of_refraction_;
    double metallic_;
    double roughness_;

    static inline Vector3d reflect(const Vector3d &in_dir, const Vector3d &normal) {
        return Normalize(in_dir - 2 * DotProduct(in_dir, normal) * normal);
    }

    static inline Vector3d refract(const Vector3d &in_dir, const Vector3d &normal, double refract_ratio) {
        double cos_theta = DotProduct(in_dir, -normal);
        Vector3d r_out_perp = refract_ratio * (in_dir + cos_theta * normal);
        Vector3d r_out_vert = -std::sqrt(1 - LengthSquared(r_out_perp)) * normal;
        return r_out_perp + r_out_vert;
    }

    // schlick approximation
    static inline double reflectance(double cosine, double ref_idx) {
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 *= r0;
        return r0 + (1 - r0) * pow(1 - cosine, 5);
    }

    // GGX ditribution for NDF
    inline double GGX(const Vector3d &m_dir, const Vector3d &normal) const {
        double cos_theta = DotProduct(m_dir, normal);
        if (cos_theta <= 0.0) {
            return 0.0;
        }
        double roughness_sqr = roughness_ * roughness_;
        double x = cos_theta * cos_theta * (roughness_sqr - 1) + 1;
        return roughness_sqr / (pi * x * x);
    }

    // Smith model for G
    inline double SmithG(const Vector3d &in_dir, const Vector3d &out_dir, const Vector3d &m_dir) const {
        double k = (roughness_ + 1) * (roughness_ + 1) / 8;
        auto schlickGGX = [&](const Vector3d &n, const Vector3d &v) {
            double n_dot_v = DotProduct(n, v);
            return n_dot_v / (n_dot_v * (1 - k) + k);
        };
        return schlickGGX(in_dir, m_dir) * schlickGGX(out_dir, m_dir);
    }
};

Vector3d Material::Eval(const Vector3d &in_dir, const Vector3d &out_dir, const Vector3d &normal) const {
    switch (type_) {
    case kDIFFUSE: {
        double cosTheta = DotProduct(normal, out_dir);
        if (cosTheta > 0.0) {
            return k_diffuse_ / pi;
        }
        return {0, 0, 0};
    }
    case kMICROFACET: {
        // Cook Torrance Microfacet Model
        Vector3d m_dir = Normalize(in_dir + out_dir);
        Vector3d F = Vector3d(1, 1, 1) * reflectance(DotProduct(in_dir, out_dir), index_of_refraction_);
        double G = SmithG(in_dir, out_dir, m_dir);
        double D = GGX(m_dir, normal);
        return (1 - metallic_) * k_diffuse_ / pi + F * G * D / (4 * DotProduct(in_dir, normal) * DotProduct(out_dir, normal));
    }
    }
    return {0, 0, 0};
}

Vector3d Material::Sample(const Vector3d &in_dir, const Vector3d &normal) const {
    switch (type_) {
    case kDIFFUSE: {
        // For diffuse material, uniform sample on the hemisphere.
        return TrRandom::UnitVec3InHemisphere(normal);
    }
    case kMICROFACET: {
        // reference to: https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
        double r = TrRandom::Double();
        double rho = TrRandom::Double(0, pi / 2);
        double theta = atan(roughness_ * sqrt(r / (1 - r)));
        return TangentToWorld(UnitVec3d(theta, rho), normal);
    }
    }
    return {0, 0, 0};
}

double Material::Pdf(const Vector3d &in_dir, const Vector3d &out_dir, const Vector3d &normal) const {
    switch (type_) {
    case kDIFFUSE: {
        // For diffuse material, the probability equal to 1 / (2 * PI).
        if (DotProduct(out_dir, normal) > 0.0) {
            return 0.5 / pi;
        } else {
            return 0.0;
        }
    }
    case kMICROFACET: {
        Vector3d m_dir = Normalize(in_dir + out_dir);
        return GGX(m_dir, normal);
    }
    }
    return 0;
}

using MaterialPtrType = shared_ptr<Material>;

#endif