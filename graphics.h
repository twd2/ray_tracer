#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <vector>
#include <cstddef>

#include "image.h"
#include "geometry.h"

#define AA_MATRIX_SIZE 5

class graphics
{
public:
    image &img;
    graphics(image &img)
        : img(img)
    {

    }

    void clear(color_t color);
    void draw_point(const vector2d &p, color_t color);
    void set_color(const vector2d &p, color_t color);
    void draw_line(const vector2d &p1, const vector2d &p2, color_t color, bool aa = false);
    void draw_polygon(const std::vector<vector2d> &v, color_t color, bool aa = false);
    void fill_polygon(const std::vector<vector2d> &v,
                      color_t edge_color, color_t inner_color, bool aa = false);
    void fill_convex_polygon(const std::vector<vector2d> &v,
                             color_t edge_color, color_t inner_color, bool aa = false);
public:
    static color_t color_hsv(unsigned short h, unsigned char s, unsigned char v);
    static void get_boundary(const std::vector<vector2d> &v,
                             vector2d &out_left_top, vector2d &out_right_bottom);
private:
    static const unsigned char aa_matrix[AA_MATRIX_SIZE][AA_MATRIX_SIZE];
    static void _set_color(image &img, const vector2d &p, color_t color);
    static void _do_aa(image &img, const vector2d &p1, const vector2d &p2,
                       const vector2d &p, color_t color);
    static void _draw_line(image &img, const vector2d &p1, const vector2d &p2,
                           color_t color, bool aa);
    static void _draw_polygon(image &img, const std::vector<vector2d> &v,
                              color_t color, bool aa);
};

#endif // _GRAPHICS_H_