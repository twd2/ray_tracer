#ifndef _BEZIER_CURVE_H_
#define _BEZIER_CURVE_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "vector3d.hpp"
#include "mesh.h"

class bezier_curve
{
public:
    std::vector<vector3df> data;
    const std::size_t n;

private:
    
public:
    bezier_curve(size_t n)
        : data(n + 1), n(n)
    {

    }
    
    vector3df &operator[](size_t i)
    {
        return data[i];
    }
    
    const vector3df &operator[](size_t i) const
    {
        return data[i];
    }

    vector3df get_point(double t) const;
    std::vector<vector3df> to_points(double dt) const;
    mesh to_rotate_surface_mesh(double dt, double dtheta) const;

    static bezier_curve load(const std::string &filename);
};

#endif // _BEZIER_CURVE_H_