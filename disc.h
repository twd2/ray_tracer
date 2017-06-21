#ifndef _DISC_H_
#define _DISC_H_

#include "object.h"
#include "plane.h"
#include "ray.h"
#include "vector3d.hpp"

class disc
    : public plane
{
public:
    const double r, r2;

    disc(const vector3df &p, double r, const vector3df &n)
        : plane(p, n), r(r), r2(r * r)
    {
        
    }

    intersect_result intersect(const ray &r) const override;
};

#endif // _DISC_H_