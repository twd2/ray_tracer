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

int main(int argc, char **argv)
{
    std::string filename = "test.png";
    if (argc >= 2)
    {
        filename = argv[1];
    }

    image img(800, 600);
    world w;
    object &ground = w.add_object(std::make_shared<plane>(
        vector3df(0.0, -1000.0, 0.0),
        vector3df(0.0, 1.0, 0.1).normalize()));
    ground.diffuse = vector3df(0.8, 0.8, 1.0) * 0.9;

    object &magic1 = w.add_object(std::make_shared<sphere>(vector3df(700.0, 250.0, -900.0), 300.0));
    magic1.diffuse = vector3df::zero;
    magic1.specular = vector3df::one * 0.2;
    magic1.shininess = 32.0;
    magic1.refractiveness = vector3df::one * 0.9; // vector3df(0.5, 1.0, 0.0) * 0.9;
    magic1.refractive_index = 1.333;
    magic1.reflectiveness = 0.05;

    object &magic2 = w.add_object(std::make_shared<sphere>(vector3df(-700.0, 250.0, -900.0), 600.0));
    magic2.diffuse = vector3df::zero;
    magic2.specular = vector3df::one * 0.2;
    magic2.shininess = 32.0;
    magic2.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    magic2.refractive_index = 1.333;
    magic2.reflectiveness = 0.05;

    object &magic3 = w.add_object(std::make_shared<sphere>(vector3df(-700.0, 250.0, -900.0), 300.0));
    magic3.diffuse = vector3df::zero;
    magic3.specular = vector3df::one * 0.2;
    magic3.shininess = 32.0;
    magic3.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    magic3.refractive_index = 1.5;
    magic3.reflectiveness = 0.05;

    object &magic4 = w.add_object(std::make_shared<sphere>(vector3df(-700.0, 250.0, -900.0), 100.0));
    magic4.diffuse = vector3df::zero;
    magic4.specular = vector3df::one * 0.2;
    magic4.shininess = 32.0;
    magic4.refractiveness = vector3df::one * 0.9; // vector3df(0.0, 0.5, 1.0) * 0.9;
    magic4.refractive_index = 1.0;
    magic4.reflectiveness = 0.05;

    w.add_object(std::make_shared<sphere>(vector3df(-1600.0, -400.0, -900.0), 200.0));
    object &small1 = w.add_object(std::make_shared<sphere>(vector3df(-1000.0, -500.0, -600.0), 10.0));
    object &small2 = w.add_object(std::make_shared<sphere>(vector3df(-850.0, -850.0, -400.0), 20.0));
    object &small3 = w.add_object(std::make_shared<sphere>(vector3df(-700.0, -900.0, -700.0), 30.0));
    small1.diffuse = small2.diffuse = small3.diffuse = vector3df(0.5, 0.5, 0.5);

    object &tri1 = w.add_object(std::make_shared<triangle>(
        vector3df(-1000.0, -500.0, -600.0),
        vector3df(-850.0, -850.0, -400.0),
        vector3df(-700.0, -900.0, -700.0)));
    tri1.diffuse = vector3df(0.5, 0.5, 0.5);

    object &tri2 = w.add_object(std::make_shared<triangle>(
        vector3df(0.0, 501.0, -5000.0),
        vector3df(-500.0, -1.0, -5000.0),
        vector3df(1000.0, -1.0, -5000.0)));
    tri2.diffuse = vector3df(0.5, 0.5, 0.5);

    /*sphere boundary(vector3df(-1000.0, -500.0, -900.0), 5000.0);
    object &fog1 = w.add_object(std::make_shared<fog>(boundary));
    fog1.diffuse = vector3df::one * 0.5;
    fog1.specular = vector3df(0.5, 0.5, 0.5);
    fog1.shininess = 32.0;
    fog1.refractiveness = vector3df::zero;
    fog1.refractive_index = 1.5;
    fog1.reflectiveness = 0.9;*/

    object &mirror = w.add_object(std::make_shared<plane>(
        vector3df(0.0, 0.0, -6000.0),
        vector3df(0.0, -0.1, 1.0).normalize()));
    mirror.reflectiveness = 0.9;
    mirror.specular = vector3df::one * 0.9;
    mirror.diffuse = vector3df::zero;

    w.lights.push_back(std::make_shared<parallel_light>(w, vector3df(-1.0, -1.0, -1.0).normalize(), vector3df(1.0, 1.0, 0.8) * 1.5));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, 600.0, -1000.0), vector3df(1.0, 1.0, 0.8)));
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(1000.0, 1500.0, 0.0), vector3df(1.0, 1.0, 0.8)));
    w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, -750.0, -400.0), vector3df(1.0, 1.0, 0.8) * 0.2));
    w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, 1000.0, 100.0), vector3df(1.0, 1.0, 0.8) * 0.2));

    camera c(w, vector3df(0.0, 0.0, 1000.0));
        
    // Performance test.
    for (int N = 1; N <= 1; ++N)
    {
        c.render(img);
        if (N % 100 == 0)
        {
            printf("Done #%d.\n", N);
        }
    }

    image out(400, 300);
    half_size(img, out);
    save_image(img, filename);
    save_image(out, "ssaa_" + filename);
    // save_image(img, to_string(10 - i) + ".png");
    show_image(img);
    return 0;
}