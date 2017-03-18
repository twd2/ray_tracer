#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <memory>
#include <cmath>

#include "lodepng.h"

#include "image.h"
#include "world.h"
#include "camera.h"
#include "plane.h"
#include "sphere.h"
#include "gui.h"

#ifndef M_PI
#define M_PI 3.141592653587979
#endif

void save_image(const image &img, const std::string &filename)
{
    lodepng::encode(filename, *img.raw, img.width, img.height, LCT_RGBA);
}

void test()
{
    vector3df v(-400, 300, 366);
    vector3df vn = v.normalize();
}

int main(int argc, char **argv)
{
    test();

    std::string filename = "test.png";
    if (argc >= 2)
    {
        filename = argv[1];
    }

    image img(800, 600);
    world w;
    camera c(w, vector3df(0.0, 0.0, 10.0));
    w.add_object(std::make_shared<plane>(vector3df(400.0, 300.0, -1.0),
                                         vector3df(0.5, 1.0, -1.0).normalize()));
    w.add_object(std::make_shared<sphere>(vector3df(0.0, 0.0, -900.0), 500.0));

    // Performance test.
    for (int N = 1; N <= 1; ++N)
    {
        c.render(img);
        if (N % 100 == 0)
        {
            printf("Done #%d.\n", N);
        }
    }

    save_image(img, filename);
    show_image(img);
    return 0;
}