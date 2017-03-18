#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ray.h"
#include "vector3d.hpp"

typedef double coeff_t;

class intersect_result
{
public:
    bool succeeded = false;
    vector3df p; // point
    vector3df n; // normal vector
    double distance;

    explicit intersect_result(bool succeeded) // Failed.
        : succeeded(succeeded)
    {
        
    }

    intersect_result(const vector3df &p, const vector3df &n, double distance)
        : succeeded(true), p(p), n(n), distance(distance)
    {
        
    }

    static const intersect_result failed;
};

class object
{
public:
    coeff_t transparent_coeff = 0.0;
    coeff_t refract_coeff = 1.0; // Refractive index.

    // First intersection.
    virtual intersect_result intersect(const ray &r)
    {
        return intersect_result::failed;
    }

    static object dummy;
};

#endif // _OBJECT_H_