#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cmath>

#include "lodepng.h"

#include "image.h"
#include "graphics.h"
#include "gui.h"

#ifndef M_PI
#define M_PI 3.141592653587979
#endif

void save_image(const image &img, const std::string &filename)
{
    lodepng::encode(filename, *img.raw, img.width, img.height, LCT_RGBA);
}

void draw(graphics &g)
{
    g.clear(color_t::white);

    g.draw_line(vector2d(-100, -100), vector2d(100, 200), color_t(255, 255, 0), true);
    g.draw_line(vector2d(10, 10), vector2d(500, 11), color_t(0, 255, 255), true);
    g.draw_line(vector2d(0, 0), vector2d(599, 599), color_t(0, 0, 0), true);
    g.draw_line(vector2d(0, 599), vector2d(599, 0), color_t(0, 0, 0), true);
    g.draw_line(vector2d(299, 0), vector2d(299, 599), color_t(0, 0, 0), true);
    g.draw_line(vector2d(0, 299), vector2d(599, 299), color_t(0, 0, 0), true);

    double r = 200.0;
    vector2d c0(299, 299);
    for (size_t i = 0; i < 360; i += 5)
    {
        double theta = (double)i / 180 * M_PI;
        g.draw_line(c0, c0 + vector2d((size_t)(r * cos(theta)), (size_t)(r * sin(theta))),
                    graphics::color_hsv(i, 255, 255), i % 10 == 0);
    }

    g.draw_line(vector2d(30, 20), vector2d(300, 400), color_t(0, 0, 0), true);

    std::vector<vector2d> v1 =
        { vector2d(80, 100), vector2d(300, 200), vector2d(200, 400), vector2d(140, 410) };
    g.fill_polygon(v1, color_t(0, 0, 0), color_t(0, 127, 255), false);

    std::vector<vector2d> v2 =
        { vector2d(200, 200), vector2d(400, 300), vector2d(300, 500), vector2d(350, 300) };
    g.fill_polygon(v2, color_t(0, 0, 0), color_t(0, 255, 127), true);

    std::vector<vector2d> v3 =
        { vector2d(500, 500), vector2d(700, 600), vector2d(600, 800), vector2d(540, 810) };
    g.fill_polygon(v3, color_t(0, 0, 0), color_t(0, 127, 255), false);

    std::vector<vector2d> v4 =
        { vector2d(-50, -100), vector2d(150, 0), vector2d(50, 200), vector2d(-10, 210) };
    g.fill_polygon(v4, color_t(0, 0, 0), color_t(0, 255, 127), true);

    std::vector<vector2d> v5 =
        { vector2d(19, 550), vector2d(20, 10), vector2d(21, 540),
          vector2d(30, 5), vector2d(50, 598) };
    g.fill_polygon(v5, color_t(255, 127, 0), color_t(255, 127, 0), true);

    std::vector<vector2d> v6 =
        { vector2d(310, 10), vector2d(310, 50), vector2d(320, 50),
          vector2d(320, 20), vector2d(330, 20), vector2d(330, 50),
          vector2d(340, 50), vector2d(340, 10) };
    g.fill_polygon(v6, color_t(0, 0, 0), color_t(255, 127, 0), false);

    g.draw_point(vector2d(10, 100), color_t(127, 127, 255));
}

int main(int argc, char **argv)
{
    std::string filename = "test.png";
    if (argc >= 2)
    {
        filename = argv[1];
    }

    image img(800, 600);
    graphics g(img);

    // Performance test.
    for (int N = 1; N <= 1; ++N)
    {
        draw(g);
        if (N % 100 == 0)
        {
            printf("Done #%d.\n", N);
        }
    }

    save_image(img, filename);
    show_image(img);
    return 0;
}