#ifndef _DISC_LIGHT_H_
#define _DISC_LIGHT_H_

#include "vector3d.hpp"
#include "light.h"

class disc_light
    : public light
{
public:
    vector3df c, n, color;
    const vector3df xn, yn;
    const double r, r2;

    disc_light(world &w, const vector3df &c, double r, const vector3df &n, const vector3df &color)
        : light(w), c(c), n(n), color(color),
          xn(n.get_vert()), yn(n.cross(xn)),
          r(r), r2(r * r)
    {

    }

    light_info illuminate(const vector3df &p) const override;

    ray emit(std::default_random_engine &engine) const override;

    vector3df flux() const override
    {
        return color;
    }
};

#endif // _DISC_LIGHT_H_