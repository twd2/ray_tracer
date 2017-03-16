#include "geometry.h"
#include <cstdio>

#define POLYGON_HALF_PI 1

static const int polygon_angle[] =
{
                      0, // 0000 
       -POLYGON_HALF_PI, // 0001
        POLYGON_HALF_PI, // 0010
    2 * POLYGON_HALF_PI, // 0011
        POLYGON_HALF_PI, // 0100
                      0, // 0101
    2 * POLYGON_HALF_PI, // 0110
       -POLYGON_HALF_PI, // 0111
       -POLYGON_HALF_PI, // 1000
    2 * POLYGON_HALF_PI, // 1001
                      0, // 1010
        POLYGON_HALF_PI, // 1011
    2 * POLYGON_HALF_PI, // 1100
        POLYGON_HALF_PI, // 1101
       -POLYGON_HALF_PI, // 1110
                      0  // 1111
};

static inline int _make_index(const vector2d &p0, const vector2d &p1)
{
    int index = 0;
    if (p0.x >= 0)
    {
        index |= 0b1000;
    }
    if (p0.y >= 0)
    {
        index |= 0b0100;
    }
    if (p1.x >= 0)
    {
        index |= 0b0010;
    }
    if (p1.y >= 0)
    {
        index |= 0b0001;
    }
    return index;
}

static inline int _get_angle(const vector2d &p0, const vector2d &p1, bool &out_online)
{
    out_online = false;
    int angle = polygon_angle[_make_index(p0, p1)];
    if (angle == 2 * POLYGON_HALF_PI)
    {
        coord_t cross = p0.cross(p1);
        if (cross < 0)
        {
            angle = -2 * POLYGON_HALF_PI;
        }
        else if (cross == 0)
        {
            out_online = true;
        }
        else
        {
            // angle = 2 * POLYGON_HALF_PI;
        }
    }
    return angle;
}

bool inside_polygon(const std::vector<vector2d> &v, const vector2d &p)
{
    int angle = 0;
    bool online = false;
    for (size_t i = 0; i < v.size() - 1; ++i)
    {
        angle += _get_angle(v[i] - p, v[i + 1] - p, online);
        if (online)
        {
            return false;
        }
    }
    angle += _get_angle(v[v.size() - 1] - p, v[0] - p, online);
    if (online)
    {
        return false;
    }
    return angle == 4 * POLYGON_HALF_PI || angle == -4 * POLYGON_HALF_PI;
}

#undef POLYGON_HALF_PI

static inline bool _get_direction(const vector2d &p0, const vector2d &p1, const vector2d &p2)
{
    return (p1 - p0).cross(p2 - p1) > 0;
}

bool is_convex_polygon(const std::vector<vector2d> &v)
{
    if (v.size() < 3)
    {
        return false;
    }

    bool direction = _get_direction(v[v.size() - 2], v[v.size() - 1], v[0]);
    if (direction != _get_direction(v[v.size() - 1], v[0], v[1]))
    {
        return false;
    }
    for (size_t i = 0; i < v.size() - 2; ++i)
    {
        if (direction != _get_direction(v[i], v[i + 1], v[i + 2]))
        {
            return false;
        }
    }
    return true;
}