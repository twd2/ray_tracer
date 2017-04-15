#ifndef _AA_CUBE_H_
#define _AA_CUBE_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

// axis-aligned cube
class aa_cube
{
public:
    vector3df p;
    vector3df size;

public:
    aa_cube(const vector3df &p, const vector3df &size);

    bool is_inside(const vector3df &p0) const
    {
        vector3df p2 = p + size;
        return p.x - eps <= p0.x && p0.x <= p2.x + eps &&
               p.y - eps <= p0.y && p0.y <= p2.y + eps &&
               p.z - eps <= p0.z && p0.z <= p2.z + eps;
    }

    intersect_result intersect(const ray &r) const;
    std::vector<intersect_result> intersect_all(const ray &r) const;

    static constexpr std::size_t front = 0, back = 1,
                                 left = 2, right = 3,
                                 top = 4, bottom = 5,
                                 none = 6;
    static const vector3df normals[6];
};

#endif // _AA_CUBE_H_