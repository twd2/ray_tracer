#include <algorithm>
#include <cstdio>

#include "graphics.h"
#include "geometry.h"

/* const unsigned char graphics::aa_matrix[AA_MATRIX_SIZE][AA_MATRIX_SIZE] =
{ { 16, 32, 16 },
  { 32, 63, 32 },
  { 16, 32, 16 } }; // sum = 255 */

/* const unsigned char graphics::aa_matrix[AA_MATRIX_SIZE][AA_MATRIX_SIZE] =
{ { 1, 4, 6, 4, 1 },
  { 4, 16, 24, 16, 4 },
  { 6, 24, 35, 24, 6 },
  { 4, 16, 24, 16, 4 },
  { 1, 4, 6, 4, 1 } }; // sum = 255 */

const unsigned char graphics::aa_matrix[AA_MATRIX_SIZE][AA_MATRIX_SIZE] =
{ { 10, 10, 10, 10, 10 },
  { 10, 10, 10, 10, 10 },
  { 10, 10, 15, 10, 10 },
  { 10, 10, 10, 10, 10 },
  { 10, 10, 10, 10, 10 } }; // sum = 255 */

void graphics::clear(color_t color)
{
    for (size_t y = 0; y < img.height; ++y)
    {
        for (size_t x = 0; x < img.width; ++x)
        {
            img.set_color(x, y, color);
        }
    }
}

void graphics::draw_point(const vector2d &p, color_t color)
{
    img.set_color(p.x, p.y, color);
}

void graphics::_set_color(image &img, const vector2d &p, color_t color)
{
    if (!color.a)
    {
        return;
    }
    img.set_color(p.x, p.y, mix_color(color, img(p.x, p.y)));
}

void graphics::set_color(const vector2d &p, color_t color)
{
    _set_color(img, p, color);
}

// 抗锯齿
// p1p2确定一个直线，通过计算直线覆盖了p的多少，来确定p点的alpha值
void graphics::_do_aa(image &img, const vector2d &p1, const vector2d &p2,
                      const vector2d &p, color_t color)
{
    if (p.x < 0 || p.x >= img.width || p.y < 0 || p.y >= img.height)
    {
        return;
    }
    double A = p1.y - p2.y, B = p2.x - p1.x, C = p1.x * p2.y - p2.x * p1.y;
    double A2_B2_d0 = 0.5 * 0.5 * (A * A + B * B);
    double s = 1.0 / (double)AA_MATRIX_SIZE;
    int alpha = 0;
    // 把一个像素分成 AA_MATRIX_SIZE^2 个小格子
    for (int row = 0; row < AA_MATRIX_SIZE; ++row)
    {
        for (int col = 0; col < AA_MATRIX_SIZE; ++col)
        {
            double sub_x = (double)p.x - 0.5 + s / 2.0 + s * (double)col,
                   sub_y = (double)p.y - 0.5 + s / 2.0 + s * (double)row;
            double distance2 = A * sub_x + B * sub_y + C;
            distance2 *= distance2;
            // distance^2 = (A * sub_x + B * sub_y + C)^2 / A * A + B * B
            if (distance2 < A2_B2_d0)
            {
                alpha += aa_matrix[row][col];
            }
        }
    }

    if (alpha)
    {
        color.a = alpha;
        _set_color(img, p, color);
    }
}

void graphics::_draw_line(image &img, const vector2d &p1, const vector2d &p2,
                          color_t color, bool aa)
{
    coord_t x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    coord_t dx = x2 - x1, dy = y2 - y1; // assert dx >= 0
    coord_t e = -dx;
    coord_t x = x1, y = y1;

    if (dx >= dy && dx >= -dy)
    {
        if (dy < 0)
        {
            e = dx;
        }
        for (coord_t i = 0; i <= dx; ++i)
        {
            if (x >= 0 && x < img.width && y >= 0 && y < img.height)
            {
                img.set_color(x, y, color);
            }
            if (aa)
            {
                _do_aa(img, p1, p2, vector2d(x, y - 1), color);
                _do_aa(img, p1, p2, vector2d(x, y + 1), color);
            }
            ++x;
            e += 2 * dy;
            if (dy >= 0 && e >= 0)
            {
                ++y;
                e -= 2 * dx;
            }
            else if (dy < 0 && e <= 0)
            {
                --y;
                e += 2 * dx;
            }
        }
    }
    else
    {
        if (y1 > y2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        dx = x2 - x1;
        dy = y2 - y1; // assert dy >= 0
        e = -dy;
        x = x1, y = y1;

        if (dx < 0)
        {
            e = dy;
        }
        for (coord_t i = 0; i <= dy; ++i)
        {
            if (x >= 0 && x < img.width && y >= 0 && y < img.height)
            {
                img.set_color(x, y, color);
            }
            if (aa)
            {
                _do_aa(img, p1, p2, vector2d(x - 1, y), color);
                _do_aa(img, p1, p2, vector2d(x + 1, y), color);
            }
            ++y;
            e += 2 * dx;
            if (dx >= 0 && e >= 0)
            {
                ++x;
                e -= 2 * dy;
            }
            else if (dx < 0 && e <= 0)
            {
                --x;
                e += 2 * dy;
            }
        }
    }
}

void graphics::draw_line(const vector2d &p1, const vector2d &p2, color_t color, bool aa)
{
    _draw_line(img, p1, p2, color, aa);
}

void graphics::_draw_polygon(image &img, const std::vector<vector2d> &v, color_t color, bool aa)
{
    if (v.size() < 3)
    {
        return;
    }

    for (size_t i = 0; i < v.size() - 1; ++i)
    {
        _draw_line(img, v[i], v[i + 1], color, aa);
    }
    _draw_line(img, v[v.size() - 1], v[0], color, aa);
}

void graphics::draw_polygon(const std::vector<vector2d> &v, color_t color, bool aa)
{
    _draw_polygon(img, v, color, aa);
}

void graphics::fill_polygon(const std::vector<vector2d> &v,
                            color_t edge_color, color_t inner_color, bool aa)
{
    if (v.size() < 3)
    {
        return;
    }

    if (is_convex_polygon(v))
    {
        fill_convex_polygon(v, edge_color, inner_color, aa);
        return;
    }
    // Else: fallback.

    vector2d left_top, right_bottom;
    get_boundary(v, left_top, right_bottom);
    if (left_top.x < 0)
    {
        left_top.x = 0;
    }
    if (left_top.x >= img.width)
    {
        return;
    }
    if (left_top.y < 0)
    {
        left_top.y = 0;
    }
    if (left_top.y >= img.height)
    {
        return;
    }
    if (right_bottom.x < 0)
    {
        return;
    }
    if (right_bottom.x >= img.width)
    {
        right_bottom.x = img.width - 1;
    }
    if (right_bottom.y < 0)
    {
        return;
    }
    if (right_bottom.y >= img.height)
    {
        right_bottom.y = img.height - 1;
    }

    for (coord_t y = left_top.y; y <= right_bottom.y; ++y)
    {
        for (coord_t x = left_top.x; x <= right_bottom.x; ++x)
        {
            if (inside_polygon(v, vector2d(x, y)))
            {
                // fill
                img.set_color(x, y, inner_color);
            }
        }
    }

    // Draw edges.
    draw_polygon(v, edge_color, aa);
}

void graphics::fill_convex_polygon(const std::vector<vector2d> &v,
                                   color_t edge_color, color_t inner_color, bool aa)
{
    if (v.size() < 3)
    {
        return;
    }

    vector2d left_top, right_bottom;
    get_boundary(v, left_top, right_bottom);

    // get edge points (rel.)
    std::vector<vector2d> rel_v;
    for (const auto &p : v)
    {
        rel_v.push_back(p - left_top);
    }
    image flags(right_bottom.x - left_top.x + 1, right_bottom.y - left_top.y + 1, 8);
    _draw_polygon(flags, rel_v, color_t::white, false);

    for (coord_t y = left_top.y; y <= right_bottom.y; ++y)
    {
        int count_edge = 0;
        for (coord_t x = left_top.x; x <= right_bottom.x; ++x)
        {
            coord_t rel_x = x - left_top.x, rel_y = y - left_top.y;
            if (flags(rel_x, rel_y, 0)) // edge
            {
                ++count_edge;
                // skip: the same line
                while (flags(rel_x, rel_y, 0) && x <= right_bottom.x)
                {
                    ++x;
                    ++rel_x;
                }
                --x;
            }
        }
        if (count_edge != 2)
        {
            continue;
        }

        bool in_inner = false;
        for (coord_t x = left_top.x; x <= right_bottom.x; ++x)
        {
            coord_t rel_x = x - left_top.x, rel_y = y - left_top.y;
            if (flags(rel_x, rel_y, 0)) // edge
            {
                in_inner = !in_inner;
                // skip: the same line
                while (flags(rel_x, rel_y, 0) && x <= right_bottom.x)
                {
                    ++x;
                    ++rel_x;
                }
                --x;
            }
            else if (in_inner && x >= 0 && x < img.width && y >= 0 && y < img.height)
            {
                // fill
                img.set_color(x, y, inner_color);
            }
        }
    }

    // draw edges
    draw_polygon(v, edge_color, aa);
}

color_t graphics::color_hsv(unsigned short h, unsigned char s, unsigned char v)
{
    unsigned short hi = (h / 60) % 6;
    double f = (double)h / 60.0 - (double)hi;
    double p = (double)v * (1.0 - (double)s / 255.0);
    double q = (double)v * (1.0 - f * (double)s / 255.0);
    double t = (double)v * (1.0 - (1 - f) * (double)s / 255.0);
    switch (hi)
    {
    case 0:
        return color_t(v, (unsigned char)t, (unsigned char)p);
    case 1:
        return color_t((unsigned char)q, v, (unsigned char)p);
    case 2:
        return color_t((unsigned char)p, v, (unsigned char)t);
    case 3:
        return color_t((unsigned char)p, (unsigned char)q, v);
    case 4:
        return color_t((unsigned char)t, (unsigned char)p, v);
    case 5:
        return color_t(v, (unsigned char)p, (unsigned char)q);
    default: // ???
        return color_t::black;
    }
}

void graphics::get_boundary(const std::vector<vector2d> &v,
                            vector2d &out_left_top, vector2d &out_right_bottom)
{
    out_left_top = v[0];
    out_right_bottom = v[0];
    for (const auto &p : v)
    {
        if (p.x < out_left_top.x)
        {
            out_left_top.x = p.x;
        }
        if (p.y < out_left_top.y)
        {
            out_left_top.y = p.y;
        }
        if (p.x > out_right_bottom.x)
        {
            out_right_bottom.x = p.x;
        }
        if (p.y > out_right_bottom.y)
        {
            out_right_bottom.y = p.y;
        }
    }
}