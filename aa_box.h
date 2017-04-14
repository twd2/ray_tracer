#ifndef _AA_BOX_H_
#define _AA_BOX_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"
#include "aa_cube.h"

// axis-aligned box
class aa_box
    : public object
{
public:
    aa_cube geometry;

public:
    aa_box(const vector3df &p, const vector3df &size)
        : geometry(p, size)
    {

    }

    intersect_result intersect(const ray &r) const override;
    std::vector<intersect_result> intersect_all(const ray &r) const override;
};

#endif // _AA_BOX_H_