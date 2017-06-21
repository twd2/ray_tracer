#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <cmath>
#include <mutex>

#include "imagef.h"
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
    std::size_t index = 0; // (optional) index
    double u, v; // (optional) surface parameters
    object *obj;
    int image_x, image_y;
    vector3df contribution;

    // for PPM
    double radius2 = 0.0;
    int photon_count = 0, new_photon_count = 0;
    vector3df flux = vector3df::zero;

    hit_point(const ray &r, object &obj, const intersect_result &ir)
        : p(ir.p), n(ir.n), ray_direction(r.direction),
          index(ir.index), u(ir.u), v(ir.v), obj(&obj),
          image_x(r.image_x), image_y(r.image_y)
    {

    }

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
    double focal_length, aperture;
    std::size_t aperture_samples = 3;
    std::size_t thread_count = 1;
    double film_width, film_height;
    std::size_t diffuse_depth = 0; // 碰到漫反射面之后还能反射多少次

private:
    std::vector<hit_point> _hit_points;
    kd_tree<hit_point> _kdt;
    std::mutex _hit_points_lock;

public:
    camera(world &w, const vector3df &location, const vector3df &front, const vector3df &up)
        : camera(w, location, front, up, 0.035, 0.0)
    {

    }

    camera(world &w, const vector3df &location, const vector3df &front, const vector3df &up,
           double focal_length, double aperture)
        : w(w), location(location), front(front), right(front.cross(up).normalize()),
          up(right.cross(front)),
          focal_length(focal_length), aperture(aperture),
          film_width(0.036), film_height(0.024)
    {

    }

    vector3df ray_trace(const ray &r, const vector3df &contribution);
    void photon_trace(const ray &r, const vector3df &contribution, double radius,
                      std::size_t depth = 0);
    void ray_trace_pass(imagef &img);
    double photon_trace_pass(int photon_count, double radius);
    void phong_estimate(imagef &img);
    void ppm_estimate(imagef &img, int photon_count);

private:
    std::vector<unsigned int> _hit_point_inside(const sphere &r) const;
    void _hit_point_inside(const sphere &r, kd_tree<hit_point>::node *node,
                           std::vector<unsigned int> &result) const;

    intersect_result _to_intersect_result(const hit_point &hp) const
    {
        return intersect_result(hp.p, hp.n, 0.0 /* TODO */, hp.u, hp.v, hp.index);
    }
};

#endif // _CAMERA_H_