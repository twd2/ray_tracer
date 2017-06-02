#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <cmath>

#include "image.h"
#include "ray.h"
#include "vector3d.hpp"
#include "world.h"
#include "kd_tree.hpp"
#include "sphere.h"

#ifndef M_PI
#define M_PI 3.141592653587979
#endif

struct hit_point
{
    vector3df p;
    vector3df n;
    vector3df ray_direction; // ray direction
    object *obj;
    int image_x, image_y;
    vector3df contribution;

    // for PPM
    double radius2 = 0.0;
    int photon_count = 0, new_photon_count = 0;
    vector3df flux = vector3df::zero;

    double get_dim(std::size_t dim)
    {
        return p.dim[dim];
    }

    const double &get_dim(std::size_t dim) const
    {
        return p.dim[dim];
    }

    aa_cube get_aabb() const
    {
        return aa_cube(p, vector3df::zero);
    }
};

class camera
{
public:
    world &w;
    vector3df location, front, right, up;
    const double focal_length, aperture;
    std::size_t aperture_samples = 4;

private:
    std::vector<hit_point> _hit_points;
    kd_tree<hit_point> _kdt;

public:
    camera(world &w, const vector3df &location, const vector3df &front, const vector3df &up)
        : camera(w, location, front, up, 367.0, 0.0)
    {

    }

    camera(world &w, const vector3df &location, const vector3df &front, const vector3df &up,
           double focal_length, double aperture)
        : w(w), location(location), front(front), right(front.cross(up).normalize()),
          up(right.cross(front)),
          focal_length(focal_length), aperture(aperture)
    {

    }

    vector3df ray_trace(const ray &r, const vector3df &contribution);
    void photon_trace(const ray &r, const vector3df &contribution, double radius);
    void ray_trace_pass(image &img);
    double photon_trace_pass(int photon_count, double radius);
    void phong_estimate(image &img);
    void ppm_estimate(image &img, int photon_count);

private:
    std::vector<unsigned int> _hit_point_inside(const sphere &r) const;
    std::vector<unsigned int>
    _hit_point_inside(const sphere &r, kd_tree<hit_point>::node *node) const;
};

#endif // _CAMERA_H_