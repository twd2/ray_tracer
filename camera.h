#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <cmath>

#include "image.h"
#include "ray.h"
#include "vector3d.hpp"
#include "world.h"

#ifndef M_PI
#define M_PI 3.141592653587979
#endif

class camera
{
public:
    world &w;
    vector3df location, front, right, up;
    const double fov_x;
    const double double_tan_fov_x_2;

    camera(world &w, const vector3df &location, const vector3df &front, const vector3df &up)
        : camera(w, location, front, up, 95.0)
    {

    }

    camera(world &w, const vector3df &location, const vector3df &front, const vector3df &up,
           double fov_x)
        : w(w), location(location), front(front), right(front.cross(up).normalize()),
          up(right.cross(front)),
          fov_x(fov_x),
          double_tan_fov_x_2(2.0 * tan(fov_x / 2.0 * M_PI / 180.0))
    {

    }

    vector3df ray_trace(const ray &r, const vector3df &contribution) const;
    void render(image &img) const;
};

#endif // _CAMERA_H_