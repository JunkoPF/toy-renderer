#include "BVH.hpp"
#include "base.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "object_list.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"

int main() {
    // Scene
    Scene scene;

    auto material_ground = make_shared<Lambertian>(Color3d(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Color3d(0.1, 0.2, 0.5));
    auto material_left = make_shared<Dielectric>(1.5);
    auto material_right = make_shared<Metal>(Color3d(0.8, 0.6, 0.2), 0.0);

    scene.AddObject(make_shared<Sphere>(Point3d(0.0, -100.5, -1.0), 100.0, material_ground));
    scene.AddObject(make_shared<Sphere>(Point3d(0.0, 0.0, -1.0), 0.5, material_center));
    scene.AddObject(make_shared<Sphere>(Point3d(-1.0, 0.0, -1.0), 0.5, material_left));
    scene.AddObject(make_shared<Sphere>(Point3d(-1.0, 0.0, -1.0), -0.45, material_left));
    scene.AddObject(make_shared<Sphere>(Point3d(1.0, 0.0, -1.0), 0.5, material_right));
    // Camera

    Camera cam(Point3d(0, 0, 1), Point3d(0, 0, -1), Vector3d(0, 1, 0), 45.0, 16.0 / 9.0, Length(Point3d(0, 0, 1) - Point3d(0, 0, -1)), 0.0);

    scene.SetCamera(cam);

    // Render

    Renderer renderer(400, 16.0 / 9.0, 100, 50);

    scene.Render(std::cout, renderer);

    std::cerr << "\nDone.\n";
    return 0;
}