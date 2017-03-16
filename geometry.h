#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <vector>

typedef std::ptrdiff_t coord_t;

class vector2d
{
public:
    coord_t x, y;

    vector2d()
        : x(0), y(0)
    {

    }

    vector2d(coord_t x, coord_t y)
        : x(x), y(y)
    {

    }

    vector2d operator-() const
    {
        return vector2d(-x, -y);
    }

    vector2d operator+(const vector2d &v2) const
    {
        return vector2d(x + v2.x, y + v2.y);
    }

    vector2d operator-(const vector2d &v2) const
    {
        return (*this) + (-v2);
    }

    coord_t cross(const vector2d &v2) const
    {
        return x * v2.y - v2.x * y;
    }
};

bool inside_polygon(const std::vector<vector2d> &v, const vector2d &p);
bool is_convex_polygon(const std::vector<vector2d> &v);

#endif // _GEOMETRY_H_