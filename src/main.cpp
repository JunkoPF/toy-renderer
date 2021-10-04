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

    auto red = make_shared<Material>(Material::kDIFFUSE, Vector3d(0.63, 0.065, 0.05), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 0.0);
    auto green = make_shared<Material>(Material::kDIFFUSE, Vector3d(0.14, 0.45, 0.091), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 0.0);
    auto white = make_shared<Material>(Material::kDIFFUSE, Vector3d(0.725, 0.71, 0.68), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 0.0);

    auto light = make_shared<Material>(Material::kDIFFUSE, Vector3d(0.725, 0.71, 0.68), Vector3d(0, 0, 0),
                                       (8.0 * Vector3d(0.747 + 0.058, 0.747 + 0.258, 0.747) + 15.6 * Vector3d(0.740 + 0.287, 0.740 + 0.160, 0.740) + 18.4 * Vector3d(0.737 + 0.642, 0.737 + 0.159, 0.737)), 0.0);

    auto list = LoadObjectModel("/home/polyethylene/toyRenderer/asset/cornellbox/floor.obj", white);
    auto tmp = LoadObjectModel("/home/polyethylene/toyRenderer/asset/cornellbox/left.obj", red);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjectModel("/home/polyethylene/toyRenderer/asset/cornellbox/right.obj", green);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjectModel("/home/polyethylene/toyRenderer/asset/cornellbox/shortbox.obj", white);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjectModel("/home/polyethylene/toyRenderer/asset/cornellbox/tallbox.obj", white);
    list.insert(list.end(), tmp.begin(), tmp.end());
    tmp = LoadObjectModel("/home/polyethylene/toyRenderer/asset/cornellbox/light.obj", light);
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
    scene.InitializeBvh();
    // Render

    Renderer renderer(400, aspect_ratio, 256, 0);
    renderer.Render(std::cout, scene);

    std::cerr << "\nDone.\n";
    return 0;
}