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
#include "point_light.h"
#include "gui.h"

#ifndef M_PI
#define M_PI 3.141592653587979
#endif

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
    /*w.add_object(std::make_shared<plane>(vector3df(400.0, 300.0, -1.0),
                                         vector3df(0.5, 1.0, -1.0).normalize()));
    w.get_object(0).diffuse = vector3df(0.5, 0.9, 0.5);*/
    w.add_object(std::make_shared<plane>(vector3df(0.0, -1000.0, 0.0),
                                         vector3df(0.0, 1.0, 0.0).normalize()));
    w.get_object(0).diffuse = vector3df(0.5, 0.5, 0.9);
    w.add_object(std::make_shared<sphere>(vector3df(0.0, 0.0, -900.0), 500.0));
    //w.get_object(1).transparency = vector3df(0.5, 0.5, 0.5);
    //w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, 0.0, -900.0), vector3df(1.0, 1.0, 1.0)));
    w.lights.push_back(std::make_shared<point_light>(w, vector3df(0.0, -750.0, -2000.0), vector3df(1.0, 1.0, 1.0)));
    w.lights.push_back(std::make_shared<point_light>(w, vector3df(-1000.0, 1500.0, -500.0), vector3df(1.0, 1.0, 1.0)));
    w.lights.push_back(std::make_shared<point_light>(w, vector3df(1000.0, 1500.0, -500.0), vector3df(1.0, 1.0, 1.0)));

    camera c(w, vector3df(0.0, 0.0, 1000));
        
    // Performance test.
    for (int N = 1; N <= 1; ++N)
    {
        c.render(img);
        if (N % 100 == 0)
        {
            printf("Done #%d.\n", N);
        }
    }

    //image out(800, 600);
    //half_size(img, out);
    save_image(img, filename);
    // save_image(img, to_string(10 - i) + ".png");
    show_image(img);
    return 0;
}