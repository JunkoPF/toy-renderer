#ifndef TR_INCLUDE_BASE_H
#define TR_INCLUDE_BASE_H

#include <cassert>
#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Common Headers

#include "matrix.hpp"
#include "ray.hpp"

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

using namespace TrMatrix::Base;
using namespace TrMatrix::Util;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
const double eps = 1e-8;

// Utility Functions

inline double DegreesToRadians(double degrees) {
    return degrees * pi / 180.0;
}

inline double clamp(double x, double min, double max) {
    assert(min < max);
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline bool nearzero(const Vector3d &vec) {
    return (fabs(vec.x()) < eps) && (fabs(vec.y()) < eps) && (fabs(vec.z()) < eps);
}

inline Vector3d Lerp(const Vector3d &lhs, const Vector3d &rhs, double k) {
    return k * lhs + (1 - k) * rhs;
}

inline Vector3d UnitVec3d(const double &theta, const double &rho) {
    double sin_theta = sin(theta);
    double cos_theta = cos(theta);
    double sin_rho = sin(rho);
    double cos_rho = cos(rho);
    return Vector3d(sin_theta * cos_rho, sin_theta * sin_rho, cos_theta);
}

inline Vector3d TangentToWorld(const Vector3d &vec, const Vector3d &N) {
    Vector3d B, C;
    if (fabs(N.x()) > fabs(N.y())) {
        C = Normalize(Vector3d(N.z(), 0.0, -N.x()));
    } else {
        C = Normalize(Vector3d(0.0, N.z(), -N.y()));
    }
    B = CrossProduct(C, N);
    return vec.x() * B + vec.y() * C + vec.z() * N;
}

// random function

namespace TrRandom {

inline double Double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double Double(const double &min, const double &max) {
    assert(min < max);
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

inline Vector3d Vec3d() {
    return Vector3d(Double(), Double(), Double());
}
inline Vector3d Vec3d(const double &min, const double &max) {
    assert(min < max);
    return Vector3d(Double(min, max), Double(min, max), Double(min, max));
}

/*
inline Vector3d random_vec3_in_unit_sphere() {
    while (true) {
        Vector3d p = random_vec3(-1, 1);
        if (lengthSquared(p) < 1) {
            return p;
        }
    }
}
*/

inline Vector3d UnitVec3d() {
    double theta = 2 * pi * Double();
    double cosPhi = 2 * Double() - 1;
    double sinPhi = sqrt(1 - cosPhi * cosPhi);

    return {sinPhi * cos(theta), sinPhi * sin(theta), cosPhi};

    //return normalize(random_vec3_in_unit_sphere());
}

inline Vector3d Vec3InUnitDisk() {
    double theta = 2 * pi * Double();
    double radius = sqrt(Double());
    return {radius * cos(theta), radius * sin(theta)};
}

inline Vector3d UnitVec3InHemisphere(const Vector3d &normal) {
    Vector3d unit_vec3 = UnitVec3d();
    return DotProduct(unit_vec3, normal) > 0 ? unit_vec3 : -unit_vec3;
}

} // namespace TrRandom

#endif