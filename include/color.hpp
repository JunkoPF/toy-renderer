#ifndef COLOR_H
#define COLOR_H

#include "base.hpp"
#include <iostream>

void WriteColor(std::ostream &os, Color3d pixel_color, int samples_per_pixel) {
    double scale = 1.0 / samples_per_pixel;
    pixel_color *= scale;

    // gamma correction
    if (pixel_color.x() < 0 || pixel_color.y() < 0 || pixel_color.z() < 0) {
        pixel_color = {0.6, 0.6, 0};
    }
    pixel_color = Sqrt(pixel_color);

    // Write the translated [0,255] value of each color component.
    os << static_cast<int>(256 * clamp(pixel_color.x(), 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(pixel_color.y(), 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(pixel_color.z(), 0.0, 0.999)) << '\n';
}

#endif