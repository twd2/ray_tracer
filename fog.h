#ifndef _FOG_H_
#define _FOG_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

class fog
    : public object
{
public:
    object &boundary;

    fog(object &boundary)
        : object(), boundary(boundary)
    {

    }

    intersect_result intersect(const ray &r) const override;
};


#endif // _SPHERE_H_