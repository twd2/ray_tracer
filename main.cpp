#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <memory>
#include <cmath>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#else
#include <sys/sysinfo.h>
#endif

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "lodepng.h"

#include "image.h"
#include "world.h"
#include "camera.h"
#include "plane.h"
#include "sphere.h"
#include "triangle.h"
#include "fog.h"
#include "point_light.h"
#include "parallel_light.h"
#include "gui.h"
#include "bezier_surface.h"
#include "bezier_curve.h"
#include "mesh.h"
#include "mesh_object.h"
#include "aa_box.h"


std::shared_ptr<image> load_image(const std::string &filename)
{
    std::vector<unsigned char> raw;
    unsigned int w, h;
    lodepng::decode(raw, w, h, filename, LCT_RGBA);
    std::shared_ptr<image> img = std::make_shared<image>(w, h);
    img->raw = raw;
    return img;
}

void save_image(const imagef &img, const std::string &filename)
{
    image img_byte = img.to_image();
    lodepng::encode(filename, img_byte.raw, img_byte.width, img_byte.height, LCT_RGBA);
}

void half_size(const imagef &in, imagef &out)
{
    for (std::size_t y = 0; y < out.height; ++y)
    {
        for (std::size_t x = 0; x < out.width; ++x)
        {
            out(x, y) = (in(2 * x, 2 * y) + in(2 * x + 1, 2 * y) +
                         in(2 * x, 2 * y + 1) + in(2 * x + 1, 2 * y + 1)) / 4.0;
        }
    }
}

std::string to_string(int i)
{
    std::stringstream ss;
    ss << i;
    std::string str;
    ss >> str;
    return str;
}

int to_int(const std::string &str)
{
    std::stringstream ss;
    ss << str;
    int i;
    ss >> i;
    return i;
}

int get_cores()
{
#if defined(_WIN32)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
#elif defined(__APPLE__)
    int count;
    size_t count_len = sizeof(count);
    sysctlbyname("hw.logicalcpu", &count, &count_len, NULL, 0);
    return count;
#else
    return get_nprocs();
#endif
}

void test_bezier()
{
    bezier_surface bs = bezier_surface::load("bezier_surface.txt");
    mesh m1 = bs.to_mesh(0.01, 0.01);
    m1.save("bs.obj");
    printf("bezier_surface\n");
    bezier_curve bc = bezier_curve::load("bezier_curve.txt");
    mesh m2 = bc.to_rotate_surface_mesh(0.01, 3.6);
    m2.save("bc.obj");
    printf("bezier_curve\n");
}

void init_world(world &w)
{
    object &left = w.add_object(std::make_shared<plane>(
        vector3df(-50.0, 0.0, 0.0),
        vector3df(1.0, 0.0, 0.0).normalize()));
    left.diffuse = vector3df(0.75, 0.25, 0.25);

    object &right = w.add_object(std::make_shared<plane>(
        vector3df(50.0, 0.0, 0.0),
        vector3df(-1.0, 0.0, 0.0).normalize()));
    right.diffuse = vector3df(0.25, 0.25, 0.75);

    object &top = w.add_object(std::make_shared<plane>(
        vector3df(0.0, 81.6, 0.0),
        vector3df(0.0, -1.0, 0.0).normalize()));
    top.diffuse = vector3df(0.75, 0.75, 0.75);

    object &bottom = w.add_object(std::make_shared<plane>(
        vector3df(0.0, 0.0, 0.0),
        vector3df(0.0, 1.0, 0.0).normalize()));
    bottom.diffuse = vector3df(0.75, 0.75, 0.75);

    object &front = w.add_object(std::make_shared<plane>(
        vector3df(0.0, 0.0, -81.6),
        vector3df(0.0, 0.0, 1.0).normalize()));
    front.diffuse = vector3df(0.75, 0.75, 0.75);

    object &glass = w.add_object(std::make_shared<sphere>(vector3df(23, 16.5, -20), 16.5));
    glass.diffuse = vector3df::zero;
    glass.specular = vector3df::one * 0.2;
    glass.shininess = 32.0;
    glass.refractiveness = vector3df(0.5, 1.0, 0.0) * 0.99;
    glass.refractive_index = 1.5;
    glass.reflectiveness = 0.99;

    triangle &twd1 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
        vector3df(-50.0 + 0.001, 20.0, -10.0),
        vector3df(-50.0 + 0.001, 20.0, -50.0),
        vector3df(-50.0 + 0.001, 60.0, -10.0))));
    twd1.bind_texture(vector3df(0.0, 0.0, 0.0),
                      vector3df(1.0 / 3.0, 0.0, 0.0),
                      vector3df(0.0, 1.0, 0.0));
    twd1.texture = load_image("texture.png");

    triangle &twd2 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
        vector3df(-50.0 + 0.001, 20.0, -50.0),
        vector3df(-50.0 + 0.001, 60.0, -50.0),
        vector3df(-50.0 + 0.001, 60.0, -10.0))));
    twd2.bind_texture(vector3df(1.0 / 3.0, 0.0, 0.0),
                      vector3df(1.0 / 3.0, 1.0, 0.0),
                      vector3df(0.0, 1.0, 0.0));
    twd2.texture = twd1.texture;

    //object &magic1 = w.add_object(std::make_shared<sphere>(vector3df(70.0, 65.0, -220.0), 60.0));
    //magic1.diffuse = vector3df::zero;
    //magic1.specular = vector3df::one * 0.2;
    //magic1.shininess = 32.0;
    //magic1.refractiveness = vector3df(0.5, 1.0, 0.0) * 0.99;
    //magic1.refractive_index = 1.333;
    //magic1.reflectiveness = 0.99;

    //object &magic2 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, 55.0, -90.0), 60.0));
    //magic2.diffuse = vector3df::zero;
    //magic2.specular = vector3df::one * 0.2;
    //magic2.shininess = 128.0;
    //magic2.refractiveness = vector3df::one * 0.99; // vector3df(0.0, 0.5, 1.0) * 0.9;
    //magic2.refractive_index = 1.333;
    //magic2.reflectiveness = 0.99;

    //object &magic3 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, 55.0, -90.0), 30.0));
    //magic3.diffuse = vector3df::zero;
    //magic3.specular = vector3df::one * 0.2;
    //magic3.shininess = 32.0;
    //magic3.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    //magic3.refractive_index = 1.5;
    //magic3.reflectiveness = 0.9;

    //object &magic4 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, 50.0, -90.0), 5.0));
    //magic4.diffuse = vector3df::zero;
    //magic4.specular = vector3df::one * 0.2;
    //magic4.shininess = 32.0;
    //magic4.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    //magic4.refractive_index = 1.0;
    //magic4.reflectiveness = 0.9;

    //object &ball = w.add_object(std::make_shared<sphere>(vector3df(-160.0, -40.0, -90.0), 40.0));
    //ball.texture = load_image("texture1.png");

    //object &small1 = w.add_object(std::make_shared<sphere>(vector3df(-100.0, -50.0, -60.0), 1.0));
    //object &small2 = w.add_object(std::make_shared<sphere>(vector3df(-85.0, -85.0, -40.0), 2.0));
    //object &small3 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, -90.0, -70.0), 3.0));
    //small1.diffuse = small2.diffuse = small3.diffuse = vector3df(0.5, 0.5, 0.5);

    //triangle &tri1 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
    //    vector3df(-100.0, -50.0, -60.0),
    //    vector3df(-85.0, -85.0, -40.0),
    //    vector3df(-70.0, -90.0, -70.0))));
    //tri1.diffuse = vector3df::zero;
    //tri1.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    //tri1.refractive_index = 1.333;
    //tri1.reflectiveness = 0.9;

    //object &tri2 = w.add_object(std::make_shared<triangle>(
    //    vector3df(0.0, 50.1, -500.0),
    //    vector3df(-50.0, -0.1, -500.0),
    //    vector3df(100.0, -0.1, -500.0)));
    //tri2.diffuse = vector3df(0.5, 0.5, 0.5);

    //triangle &tri3 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
    //    vector3df(0.0, 80.0, -100.0),
    //    vector3df(-50.0, -0.1, -100.0),
    //    vector3df(100.0, -0.1, -100.0))));
    //tri3.bind_texture(vector3df(0.5, 1.0, 0.0),
    //                  vector3df(0.0, 0.0, 0.0),
    //                  vector3df(1.0, 0.0, 0.0));
    //tri3.texture = ball.texture;

    //bezier_curve bc = bezier_curve::load("bezier_curve.txt");
    //for (auto &v : bc.data)
    //{
    //    v = v * 10.0;
    //    //v.x *= -1.0;
    //    v.y *= -1.0;
    //    // v.y += 40.0;
    //}
    //std::reverse(bc.data.begin(), bc.data.end());
    //mesh m = bc.to_rotate_surface_mesh(0.01, 3.6);
    ///*for (auto &v : m.vertices)
    //{
    //    v = v * 10.0;
    //    v.x *= -1.0;
    //    v.y *= -1.0;
    //}*/

    //// test
    //// intersect_result ir = bc.intersect(ray(vector3df(0.0, -10.0, 147.0), vector3df(0.0, 0.01, -1.0).normalize(), 0, 0), 1250.0, 0.1, 1.0);

    //mesh_object &mo = static_cast<mesh_object &>(w.add_object(std::make_shared<mesh_object>(m)));
    //mo.diffuse = vector3df(0.24, 0.48, 0.53);
    //mo.smooth = true;
    ////mo.diffuse = vector3df::zero;
    ////mo.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    ////mo.refractive_index = 1.333;
    ////mo.reflectiveness = 0.9;
    //mo.texture = ball.texture;
    //object &box = w.add_object(std::make_shared<aa_box>(vector3df(60.0, -50.0, -220.0),
    //                                                    vector3df(20.0, 30.0, 190.0)));
    //box.diffuse = vector3df(0.24, 0.48, 0.53);
    //box.diffuse = vector3df::zero;
    //box.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    //box.refractive_index = 1.5;
    //box.reflectiveness = 0.5;
    //object &sb1 = w.add_object(std::make_shared<sphere>(vector3df(60.0, -50.0, -220.0), 5.0));
    //object &sb2 = w.add_object(std::make_shared<sphere>(vector3df(80.0, -20.0, -30.0), 5.0));
    //sb1.diffuse = sb2.diffuse = vector3df(0.5, 0.5, 0.5);

    ///*sphere boundary(vector3df(-1000.0, -500.0, -900.0), 5000.0);
    //object &fog1 = w.add_object(std::make_shared<fog>(boundary));
    //fog1.diffuse = vector3df::one * 0.5;
    //fog1.specular = vector3df(0.5, 0.5, 0.5);
    //fog1.shininess = 32.0;
    //fog1.refractiveness = vector3df::zero;
    //fog1.refractive_index = 1.5;
    //fog1.reflectiveness = 0.9;*/

    //object &mirror = w.add_object(std::make_shared<plane>(
    //    vector3df(0.0, 0.0, -600.0),
    //    vector3df(0.0, -0.1, 1.0).normalize()));
    //mirror.reflectiveness = 0.5;
    //mirror.specular = vector3df::one * 0.5;
    //mirror.diffuse = vector3df(0.25, 0.25, 0.75); // vector3df::zero;

    /*object &floor = w.add_object(std::make_shared<plane>(
        vector3df(0.0, -400.0, 0.0),
        vector3df(0.0, 1.0, 0.1).normalize()));
    floor.specular = vector3df::zero;
    floor.diffuse = vector3df::zero;
    // floor.diffuse = vector3df(0.8, 0.8, 1.0) * 0.9;
    floor.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    floor.refractive_index = 1.333;
    floor.reflectiveness = 0.9;*/

    //w.lights.push_back(std::make_shared<parallel_light>(w, vector3df(-1.0, -1.0, -1.0).normalize(), vector3df(1.0, 1.0, 0.8) * 1.5));
    w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, 71.6, -40.0), vector3df(1.0, 1.0, 0.8)));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(70.0, 60.0, 100.0), vector3df(1.0, 1.0, 0.8)));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(1000.0, 1500.0, 0.0), vector3df(1.0, 1.0, 0.8)));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, -750.0, -400.0), vector3df(1.0, 1.0, 0.8) * 0.9));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, 1000.0, 100.0), vector3df(1.0, 1.0, 0.8) * 0.9));

    /*std::size_t light_samples = 4;
    double light_samples2 = light_samples * light_samples;
    for (int j = 0; j < light_samples; ++j)
    {
        for (int i = 0; i < light_samples; ++i)
        {
            w.lights.push_back(std::make_shared<point_light>(w, vector3df(i * 2.0, 100.0, 10.0 - j * 2.0),
                                                             vector3df(1.0, 1.0, 0.8) * (1.3 / light_samples2)));
        }
    }*/
}

int main(int argc, char **argv)
{
    test_bezier();
    // return 0;

    std::size_t thread_count = get_cores();
    std::string filename = "test.png";
    if (argc >= 2)
    {
        filename = argv[1];
    }

    if (argc >= 3)
    {
        thread_count = to_int(argv[2]);
    }

    printf("Using %" PRId64 " threads.\n", thread_count);

    world w;
    init_world(w);

    imagef img(800, 600);
    camera c(w, vector3df(0.0, 50.0, 147.0), vector3df(0.0, -0.05, -1.0).normalize(), vector3df(0.0, 1.0, 0.0));
    c.thread_count = thread_count;
    c.aperture = 0.0;
    c.ray_trace_pass(img);

    // PPM
    int photon_count = 0;
    constexpr int photons = 100000;
    printf("Iteration (initial)\n");
    double radius = c.photon_trace_pass(photons, 10.0);
    photon_count += photons;
    for (int i = 0; i < 1000; ++i)
    {
        printf("Iteration %d\n", i + 1);
        radius = c.photon_trace_pass(photons, radius);
        photon_count += photons;
        if (i == 0 || (i + 1) % 10 == 0)
        {
            imagef img_copied = img;
            c.ppm_estimate(img_copied, photon_count);
            save_image(img_copied, filename + "." + to_string(i + 1) + ".png");
        }
    }
    c.ppm_estimate(img, photon_count); // */

    // Phong
    //c.phong_estimate(img);

    imagef out(img.width / 2, img.height / 2);
    half_size(img, out);
    save_image(img, filename);
    save_image(out, "ssaa_" + filename);
    return 0;
}