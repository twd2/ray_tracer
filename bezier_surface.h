#ifndef _BEZIER_SURFACE_H_
#define _BEZIER_SURFACE_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "vector3d.hpp"
#include "mesh.h"

class bezier_surface
{
public:
    std::vector<vector3df> data;
    const std::size_t width, height;
    //                m + 1, n + 1

private:
    
public:
    bezier_surface(size_t width, size_t height)
        : data(width * height), width(width), height(height)
    {

    }
    
    vector3df &operator()(size_t x, size_t y)
    {
        return data[y * width + x];
    }
    
    const vector3df &operator()(size_t x, size_t y) const
    {
        return data[y * width + x];
    }

    vector3df get_point(double u, double v) const;
    mesh to_mesh(double du, double dv) const;

    static bezier_surface load(const std::string &filename);
};

#endif // _BEZIER_SURFACE_H_