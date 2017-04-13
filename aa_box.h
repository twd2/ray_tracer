#ifndef _AA_BOX_H_
#define _AA_BOX_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

#include "plane.h"

// axis-aligned box
class aa_box
    : public object
{
public:
    const vector3df p;
    const vector3df size;
    const plane planes[6];

public:
    aa_box(const vector3df &p, const vector3df &size);

    bool is_inside(const vector3df &p0) const
    {
        vector3df p2 = p + size;
        return p.x <= p0.x && p0.x <= p2.x && 
               p.y <= p0.y && p0.y <= p2.y &&
               p.z <= p0.z && p0.z <= p2.z;
    }

    intersect_result intersect(const ray &r) const override;

    static constexpr std::size_t front = 0, back = 1,
                                 left = 2, right = 3,
                                 top = 4, bottom = 5,
                                 none = 6;
};


#endif // _AA_BOX_H_