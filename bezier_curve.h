#ifndef _BEZIER_CURVE_H_
#define _BEZIER_CURVE_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"
#include "mesh.h"

class bezier_curve
{
    // x(t, theta) = x(t) * cos(theta)
    // y(t, theta) = y(t)
    // z(t, theta) = -x(t) * sin(theta)
public:
    std::vector<vector3df> data;
    const std::size_t n;

private:
    mutable std::unique_ptr<bezier_curve> _cache = nullptr; // for derivatives

public:
    bezier_curve(const bezier_curve &b)
        : data(b.data), _cache(nullptr), n(b.n)
    {

    }

    explicit bezier_curve(size_t n)
        : data(n + 1), _cache(nullptr), n(n)
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
    vector3df d_dt(double t) const;
    std::vector<vector3df> to_points(double dt) const;
    std::vector<vector3df> to_tangents(double dt) const;
    mesh to_rotate_surface_mesh(double dt, double dtheta) const;
    vector3df d_dt(double t, double theta) const;
    vector3df d_dtheta(double t, double theta) const;
    void get(double t, double theta, vector3df &out_point,
             vector3df &out_d_dt, vector3df &out_d_dtheta) const;

    static bezier_curve load(const std::string &filename);
};

#endif // _BEZIER_CURVE_H_