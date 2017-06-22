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
#include "disc.h"
#include "sphere.h"
#include "triangle.h"
#include "fog.h"
#include "point_light.h"
#include "parallel_light.h"
#include "sphere_light.h"
#include "disc_light.h"
#include "gui.h"
#include "bezier_surface.h"
#include "bezier_curve.h"
#include "mesh.h"
#include "mesh_object.h"
#include "rotate_bezier.h"
#include "aa_box.h"

// #define DEBUG_PHONG_MODEL 1

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
    printf("bezier_surface...\n");
    bezier_surface bs = bezier_surface::load("bezier_surface.txt");
    mesh m1 = bs.to_mesh(0.01, 0.01);
    m1.save("bezier_surface.obj");

    printf("bezier_curve...\n");
    bezier_curve bc = bezier_curve::load("bezier_curve.txt");
    mesh m2 = bc.to_rotate_surface_mesh(0.01, 3.6);
    m2.save("bezier_curve.obj");
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

    object &glass = w.add_object(std::make_shared<sphere>(vector3df(23, 12.5, 0.0), 12.5));
    glass.diffuse = vector3df::zero;
    glass.specular = vector3df::one * 0.2;
    glass.shininess = 32.0;
    glass.refractiveness = vector3df(0.5, 1.0, 0.0) * 0.99;
    glass.refractive_index = 1.5;
    glass.reflectiveness = 0.99;

    object &mirror = w.add_object(std::make_shared<sphere>(vector3df(-23, 16.5, -50), 16.5));
    mirror.diffuse = vector3df::zero;
    mirror.specular = vector3df::one * 0.2;
    mirror.shininess = 32.0;
    mirror.reflectiveness = 0.99;

    sphere &bump = static_cast<sphere &>(w.add_object(std::make_shared<sphere>(
        vector3df(-39, 7.0, -30.0), 7.0)));
    bump.diffuse = vector3df::zero;
    bump.specular = vector3df::one * 0.2;
    bump.shininess = 32.0;
    bump.refractive_index = 1.5;
    bump.reflectiveness = 0.99;
    bump.bump_texture = load_image("texture/bump_texture.png");

    triangle &twd1 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
        vector3df(-50.0 + 0.001, 20.0, 10.0),
        vector3df(-50.0 + 0.001, 20.0, -30.0),
        vector3df(-50.0 + 0.001, 60.0, 10.0))));
    twd1.bind_texture(vector3df(0.0, 0.0, 0.0),
                      vector3df(1.0 / 3.0, 0.0, 0.0),
                      vector3df(0.0, 1.0, 0.0));
    twd1.texture = load_image("texture/texture.png");

    triangle &twd2 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
        vector3df(-50.0 + 0.001, 20.0, -30.0),
        vector3df(-50.0 + 0.001, 60.0, -30.0),
        vector3df(-50.0 + 0.001, 60.0, 10.0))));
    twd2.bind_texture(vector3df(1.0 / 3.0, 0.0, 0.0),
                      vector3df(1.0 / 3.0, 1.0, 0.0),
                      vector3df(0.0, 1.0, 0.0));
    twd2.texture = twd1.texture;

    bezier_curve bezier_vase = bezier_curve::load("bezier_curve.txt");
    for (auto &v : bezier_vase.data)
    {
        v = v * 5.0;
        v.y *= -1.0;
    }
    std::reverse(bezier_vase.data.begin(), bezier_vase.data.end());
    rotate_bezier &vase = static_cast<rotate_bezier &>(w.add_object(std::make_shared<rotate_bezier>(
        vector3df(20.0, 70.0, -60.0),
        bezier_vase,
        0.02, 3.6)));
    vase.native = true;
    vase.reflectiveness = 0.1;
    vase.texture = load_image("texture/vase.png");

    object &table = w.add_object(std::make_shared<aa_box>(
        vector3df(0.0, 27.0, -80.0),
        vector3df(40.0, 3.0, 40)));
    table.diffuse = vector3df::zero;
    table.refractiveness = vector3df::one * 0.8;
    table.refractive_index = 1.5;
    table.reflectiveness = 0.8;

    object &stick = w.add_object(std::make_shared<aa_box>(
        vector3df(20.0 - 2.0, 0.0, -60.0 - 2.0),
        vector3df(4.0, 27.0 - 0.001, 4.0)));
    stick.diffuse = vector3df(170, 106, 66) / 255.0;
    stick.specular = vector3df::one * 0.2;
    stick.shininess = 32.0;

    w.lights.push_back(std::make_shared<disc_light>(
        w,
        vector3df(-20.0, 81.6 - 0.001, 0.0),
        10.0,
        vector3df(0.0, -1.0, 0.0),
        vector3df(1.0, 1.0, 0.8)));

    /*w.lights.push_back(std::make_shared<point_light>(
        w,
        vector3df(-20.0, 81.6 - 0.001, 100.0),
        vector3df(1.0, 1.0, 0.8)));*/
}

int main(int argc, char **argv)
{
    test_bezier();

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
    camera c(w, vector3df(0.0, 50.0, 167.0), vector3df(0.0, -0.05, -1.0).normalize(), vector3df(0.0, 1.0, 0.0));
    c.thread_count = thread_count;
    //c.aperture = 4.0;
    c.focal_length = 227;
    c.aperture_samples = 2;
    //c.diffuse_depth = 1;
    c.film_width = 800.0 * 0.2 * 227 / 167;
    c.film_height = 600.0 * 0.2 * 227 / 167;
    c.ray_trace_pass(img);

#ifndef DEBUG_PHONG_MODEL
    // PPM
    int photon_count = 0;
    constexpr int photons = 100000;
    printf("Iteration (initial)\n");
    double radius = c.photon_trace_pass(photons, 1.0);
    photon_count += photons;
    for (int i = 0; i < 5000; ++i)
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
    c.ppm_estimate(img, photon_count);
#endif

#ifdef DEBUG_PHONG_MODEL
    // Phong
    c.phong_estimate(img);
#endif

    imagef out(img.width / 2, img.height / 2);
    half_size(img, out);
    save_image(img, filename);
    save_image(out, "ssaa_" + filename);
    return 0;
}