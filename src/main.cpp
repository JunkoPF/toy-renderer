#include "BVH.hpp"
#include "base.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "object_list.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "traingle.hpp"

int main() {
    // Scene
    Scene scene;

    //scene.AddObject(make_shared<Sphere>(Point3d(0.0, -100.5, -1.0), 100.0, material_ground));
    //scene.AddObject(make_shared<Sphere>(Point3d(0.0, 0.0, -1.0), 0.5, material_center));
    //scene.AddObject(make_shared<Sphere>(Point3d(-1.0, 0.0, -1.0), 0.5, material_left));
    //scene.AddObject(make_shared<Sphere>(Point3d(-1.0, 0.0, -1.0), -0.45, material_left));
    //scene.AddObject(make_shared<Sphere>(Point3d(1.0, 0.0, -1.0), 0.5, material_right));
    // scene.AddObject(make_shared<Triangle>(Point3d(-1.0, -1.0, -1.0),
    //                                       Point3d(1.0, -1.0, -1.0),
    //                                       Point3d(0.0, 1.0, -1.0),
    //                                           material_center));

    auto material_floor = make_shared<Lambertian>(Color3d(0.3, 0.3, 0.3));
    auto material_left = make_shared<Metal>(Color3d(0.2, 0.2, 0.8), 0.1);
    auto material_right = make_shared<Lambertian>(Color3d(0.8, 0.2, 0.2));
    auto material_shortbox = make_shared<Metal>(Color3d(0.2, 0.8, 0.8), 0.1);
    auto material_tallbox = make_shared<Lambertian>(Color3d(0.1, 0.6, 0.1));

    auto list = LoadObjModel("/home/polyethylene/toyRenderer/asset/cornellbox/floor.obj", material_floor);
    auto tmp = LoadObjModel("/home/polyethylene/toyRenderer/asset/cornellbox/left.obj", material_left);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjModel("/home/polyethylene/toyRenderer/asset/cornellbox/right.obj", material_right);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjModel("/home/polyethylene/toyRenderer/asset/cornellbox/shortbox.obj", material_shortbox);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjModel("/home/polyethylene/toyRenderer/asset/cornellbox/tallbox.obj", material_tallbox);
    list.insert(list.end(), tmp.begin(), tmp.end());
    for (auto &elem : list) {
        scene.AddObject(elem);
    }

    // Camera
    Point3d view_point(278, 273, -550);
    Point3d look_at_point(278, 273, 0);
    double fov = 20.0;
    double aspect_ratio = 1.0;

    Camera cam(view_point, look_at_point, Vector3d(0, 1, 0), 50.0, aspect_ratio, 0.035, 0.0);

    scene.SetCamera(cam);

    // Render

    Renderer renderer(400, aspect_ratio, 100, 50);
    scene.Render(std::cout, renderer);

    std::cerr << "\nDone.\n";
    return 0;
}