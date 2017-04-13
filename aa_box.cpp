#include <cstddef>
#include <cstdio>

#include "aa_box.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

aa_box::aa_box(const vector3df &p, const vector3df &size)
    : object(), p(p), size(size),
      planes
      {
          plane(p + size, vector3df(0.0, 0.0,  1.0)), // front
          plane(p,        vector3df(0.0, 0.0, -1.0)), // back
          plane(p,        vector3df(-1.0, 0.0, 0.0)), // left
          plane(p + size, vector3df( 1.0, 0.0, 0.0)), // right
          plane(p + size, vector3df(0.0,  1.0, 0.0)), // top
          plane(p,        vector3df(0.0, -1.0, 0.0))  // bottom
      }
{

}

intersect_result aa_box::intersect(const ray &r) const
{
    // woo algorithm
    std::size_t front_back, left_right, top_bottom;
    if (!is_inside(r.origin))
    {
        if (r.direction.x > eps)
        {
            left_right = left;
        }
        else if (r.direction.x < -eps)
        {
            left_right = right;
        }
        else
        {
            left_right = none;
        }

        if (r.direction.y > eps)
        {
            top_bottom = bottom;
        }
        else if (r.direction.y < -eps)
        {
            top_bottom = top;
        }
        else
        {
            top_bottom = none;
        }

        if (r.direction.z > eps)
        {
            front_back = back;
        }
        else if (r.direction.z < -eps)
        {
            front_back = front;
        }
        else
        {
            front_back = none;
        }
    }
    else
    {
        if (r.direction.x > eps)
        {
            left_right = right;
        }
        else if (r.direction.x < -eps)
        {
            left_right = left;
        }
        else
        {
            left_right = none;
        }

        if (r.direction.y > eps)
        {
            top_bottom = top;
        }
        else if (r.direction.y < -eps)
        {
            top_bottom = bottom;
        }
        else
        {
            top_bottom = none;
        }

        if (r.direction.z > eps)
        {
            front_back = front;
        }
        else if (r.direction.z < -eps)
        {
            front_back = back;
        }
        else
        {
            front_back = none;
        }
    }

    intersect_result ir[3] { intersect_result::failed,
                             intersect_result::failed,
                             intersect_result::failed };

    if (front_back != none)
    {
        ir[0] = planes[front_back].intersect(r);
    }
    if (left_right != none)
    {
        ir[1] = planes[left_right].intersect(r);
    }
    if (top_bottom != none)
    {
        ir[2] = planes[top_bottom].intersect(r);
    }

    std::size_t furthest_index = 0;
    for (std::size_t i = 1; i < 3; ++i)
    {
        if (ir[i].succeeded)
        {
            if (!ir[furthest_index].succeeded || ir[furthest_index].distance < ir[i].distance)
            {
                furthest_index = i;
            }
        }
    }

    intersect_result &result = ir[furthest_index];

    if (!result.succeeded || result.distance < eps)
    {
        return intersect_result::failed;
    }

    vector3df p2 = p + size;
    if (furthest_index == 0) // front_back
    {
        if (!(p.x < result.p.x && result.p.x < p2.x &&
              p.y < result.p.y && result.p.y < p2.y))
        {
            return intersect_result::failed;
        }
    }
    else if (furthest_index == 1) // left_right
    {
        if (!(p.y < result.p.y && result.p.y < p2.y &&
              p.z < result.p.z && result.p.z < p2.z))
        {
            return intersect_result::failed;
        }
    }
    else if (furthest_index == 2) // top_bottom
    {
        if (!(p.x < result.p.x && result.p.x < p2.x &&
              p.z < result.p.z && result.p.z < p2.z))
        {
            return intersect_result::failed;
        }
    }
    else
    {
        return intersect_result::failed;
    }

    return ir[furthest_index];
}