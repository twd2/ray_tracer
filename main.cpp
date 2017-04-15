#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <memory>
#include <cmath>

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


void save_image(const image &img, const std::string &filename)
{
    lodepng::encode(filename, *img.raw, img.width, img.height, LCT_RGBA);
}

void half_size(const image &in, image &out)
{
    for (std::size_t y = 0; y < out.height; ++y)
    {
        for (std::size_t x = 0; x < out.width; ++x)
        {
            const color_t &a = in(2 * x, 2 * y),
                          &b = in(2 * x + 1, 2 * y),
                          &c = in(2 * x, 2 * y + 1),
                          &d = in(2 * x + 1, 2 * y + 1);
            out.set_color(x, y, color_t(((int)a.r + (int)b.r + (int)c.r + (int)d.r) / 4,
                                        ((int)a.g + (int)b.g + (int)c.g + (int)d.g) / 4,
                                        ((int)a.b + (int)b.b + (int)c.b + (int)d.b) / 4));
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

int main(int argc, char **argv)
{
    test_bezier();
    // return 0;

    std::string filename = "test.png";
    if (argc >= 2)
    {
        filename = argv[1];
    }

    image img(800, 600);
    world w;
    object &ground = w.add_object(std::make_shared<plane>(
        vector3df(0.0, -100.0, 0.0),
        vector3df(0.0, 1.0, 0.1).normalize()));
    ground.diffuse = vector3df(0.8, 0.8, 1.0) * 0.9;

    object &magic1 = w.add_object(std::make_shared<sphere>(vector3df(70.0, 65.0, -220.0), 60.0));
    magic1.diffuse = vector3df::zero;
    magic1.specular = vector3df::one * 0.2;
    magic1.shininess = 32.0;
    magic1.refractiveness = vector3df::one * 0.99; // vector3df(0.5, 1.0, 0.0) * 0.9;
    magic1.refractive_index = 1.333;
    magic1.reflectiveness = 0.99;

    object &magic2 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, 55.0, -90.0), 60.0));
    magic2.diffuse = vector3df::zero;
    magic2.specular = vector3df::one * 0.2;
    magic2.shininess = 128.0;
    magic2.refractiveness = vector3df::one * 0.99; // vector3df(0.0, 0.5, 1.0) * 0.9;
    magic2.refractive_index = 1.333;
    magic2.reflectiveness = 0.99;

    object &magic3 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, 55.0, -90.0), 30.0));
    magic3.diffuse = vector3df::zero;
    magic3.specular = vector3df::one * 0.2;
    magic3.shininess = 32.0;
    magic3.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    magic3.refractive_index = 1.5;
    magic3.reflectiveness = 0.9;

    object &magic4 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, 50.0, -90.0), 5.0));
    magic4.diffuse = vector3df::zero;
    magic4.specular = vector3df::one * 0.2;
    magic4.shininess = 32.0;
    magic4.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    magic4.refractive_index = 1.0;
    magic4.reflectiveness = 0.9;

    w.add_object(std::make_shared<sphere>(vector3df(-160.0, -40.0, -90.0), 20.0));
    object &small1 = w.add_object(std::make_shared<sphere>(vector3df(-100.0, -50.0, -60.0), 1.0));
    object &small2 = w.add_object(std::make_shared<sphere>(vector3df(-85.0, -85.0, -40.0), 2.0));
    object &small3 = w.add_object(std::make_shared<sphere>(vector3df(-70.0, -90.0, -70.0), 3.0));
    small1.diffuse = small2.diffuse = small3.diffuse = vector3df(0.5, 0.5, 0.5);

    triangle &tri1 = static_cast<triangle &>(w.add_object(std::make_shared<triangle>(
        vector3df(-100.0, -50.0, -60.0),
        vector3df(-85.0, -85.0, -40.0),
        vector3df(-70.0, -90.0, -70.0))));
    tri1.diffuse = vector3df::zero;
    tri1.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    tri1.refractive_index = 1.333;
    tri1.reflectiveness = 0.9;

    object &tri2 = w.add_object(std::make_shared<triangle>(
        vector3df(0.0, 50.1, -500.0),
        vector3df(-50.0, -0.1, -500.0),
        vector3df(100.0, -0.1, -500.0)));
    tri2.diffuse = vector3df(0.5, 0.5, 0.5);

    bezier_curve bc = bezier_curve::load("bezier_curve.txt");
    mesh m = bc.to_rotate_surface_mesh(0.01, 0.36);
    for (auto &v : m.vertices)
    {
        v = v * 10.0;
        v.x *= -1.0;
        v.y *= -1.0;
        // v.y += 40.0;
    }
    object &mo = w.add_object(std::make_shared<mesh_object>(m));
    mo.diffuse = vector3df(0.24, 0.48, 0.53);
    /*mo.diffuse = vector3df::zero;
    mo.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    mo.refractive_index = 1.333;
    mo.reflectiveness = 0.9;*/
    object &box = w.add_object(std::make_shared<aa_box>(vector3df(60.0, -50.0, -220.0),
                                                        vector3df(20.0, 30.0, 190.0)));
    box.diffuse = vector3df(0.24, 0.48, 0.53);
    box.diffuse = vector3df::zero;
    box.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    box.refractive_index = 1.5;
    box.reflectiveness = 0.5;
    object &sb1 = w.add_object(std::make_shared<sphere>(vector3df(60.0, -50.0, -220.0), 5.0));
    object &sb2 = w.add_object(std::make_shared<sphere>(vector3df(80.0, -20.0, -30.0), 5.0));
    sb1.diffuse = sb2.diffuse = vector3df(0.5, 0.5, 0.5);

    /*sphere boundary(vector3df(-1000.0, -500.0, -900.0), 5000.0);
    object &fog1 = w.add_object(std::make_shared<fog>(boundary));
    fog1.diffuse = vector3df::one * 0.5;
    fog1.specular = vector3df(0.5, 0.5, 0.5);
    fog1.shininess = 32.0;
    fog1.refractiveness = vector3df::zero;
    fog1.refractive_index = 1.5;
    fog1.reflectiveness = 0.9;*/

    object &mirror = w.add_object(std::make_shared<plane>(
        vector3df(0.0, 0.0, -600.0),
        vector3df(0.0, -0.1, 1.0).normalize()));
    mirror.reflectiveness = 0.05;
    mirror.specular = vector3df::one * 0.5;
    mirror.diffuse = vector3df(0.25, 0.25, 0.75); // vector3df::zero;

    /*object &floor = w.add_object(std::make_shared<plane>(
        vector3df(0.0, -400.0, 0.0),
        vector3df(0.0, 1.0, 0.1).normalize()));
    floor.specular = vector3df::zero;
    floor.diffuse = vector3df::zero;
    // floor.diffuse = vector3df(0.8, 0.8, 1.0) * 0.9;
    floor.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    floor.refractive_index = 1.333;
    floor.reflectiveness = 0.9;*/

    w.lights.push_back(std::make_shared<parallel_light>(w, vector3df(-1.0, -1.0, -1.0).normalize(), vector3df(1.0, 1.0, 0.8) * 1.5));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(-100.0, 60.0, -100.0), vector3df(1.0, 1.0, 0.8)));
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

    camera c(w, vector3df(0.0, -1.0, 147.0), vector3df(0.0, 0.05, -1.0).normalize(), vector3df(0.0, 1.0, 0.0));
        
    // Performance test.
    for (int N = 1; N <= 1; ++N)
    {
        c.render(img);
        if (N % 100 == 0)
        {
            printf("Done #%d.\n", N);
        }
    }

    image out(img.width / 2, img.height / 2);
    half_size(img, out);
    save_image(img, filename);
    save_image(out, "ssaa_" + filename);
    // save_image(img, to_string(10 - i) + ".png");
    // show_image(img);
    return 0;
}