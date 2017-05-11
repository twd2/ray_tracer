#include <cstddef>
#include <cstdio>

#include "aa_cube.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

const vector3df aa_cube::normals[6] =
{
    vector3df(0.0, 0.0,  1.0), // front
    vector3df(0.0, 0.0, -1.0), // back
    vector3df(-1.0, 0.0, 0.0), // left
    vector3df( 1.0, 0.0, 0.0), // right
    vector3df(0.0,  1.0, 0.0), // top
    vector3df(0.0, -1.0, 0.0)  // bottom
};

aa_cube::aa_cube(const vector3df &p, const vector3df &size)
    : p(p), size(size)
{

}

intersect_result aa_cube::intersect(const ray &r) const
{
    // woo algorithm

    // find planes to intersect
    std::size_t front_back, left_right, top_bottom;
    bool inside = false;
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
        inside = true;
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

    // intersect with 3 planes
    vector3df p2 = p + size;
    double intersection[3] { -1.0, -1.0, -1.0 };
    if (front_back != none)
    {
        if (front_back == front)
        {
            intersection[0] = (p2.z - r.origin.z) / r.direction.z;
        }
        if (front_back == back)
        {
            intersection[0] = (p.z - r.origin.z) / r.direction.z;
        }
    }
    if (left_right != none)
    {
        if (left_right == left)
        {
            intersection[1] = (p.x - r.origin.x) / r.direction.x;
        }
        if (left_right == right)
        {
            intersection[1] = (p2.x - r.origin.x) / r.direction.x;
        }
    }
    if (top_bottom != none)
    {
        if (top_bottom == top)
        {
            intersection[2] = (p2.y - r.origin.y) / r.direction.y;
        }
        if (top_bottom == bottom)
        {
            intersection[2] = (p.y - r.origin.y) / r.direction.y;
        }
    }

    // select the furthest one
    std::size_t furthest_index = 0;
    for (std::size_t i = 1; i < 3; ++i)
    {
        if (!inside)
        {
            if (intersection[furthest_index] < intersection[i])
            {
                furthest_index = i;
            }
        }
        else
        {
            if (intersection[furthest_index] > intersection[i])
            {
                furthest_index = i;
            }
        }
    }

    // further check
    double &t = intersection[furthest_index];

    if (t <= eps)
    {
        return intersect_result::failed;
    }

    intersect_result ir(r.origin + r.direction * t, vector3df::zero, t);
    //                                              would be filled later

    if (furthest_index == 0) // front_back
    {
        if (!(p.x - eps < ir.p.x && ir.p.x < p2.x + eps &&
              p.y - eps < ir.p.y && ir.p.y < p2.y + eps))
        {
            return intersect_result::failed;
        }
        ir.n = normals[front_back];
    }
    else if (furthest_index == 1) // left_right
    {
        if (!(p.y - eps < ir.p.y && ir.p.y < p2.y + eps &&
              p.z - eps < ir.p.z && ir.p.z < p2.z + eps))
        {
            return intersect_result::failed;
        }
        ir.n = normals[left_right];
    }
    else if (furthest_index == 2) // top_bottom
    {
        if (!(p.x - eps < ir.p.x && ir.p.x < p2.x + eps &&
              p.z - eps < ir.p.z && ir.p.z < p2.z + eps))
        {
            return intersect_result::failed;
        }
        ir.n = normals[top_bottom];
    }
    else
    {
        return intersect_result::failed;
    }

    return ir;
}

std::vector<intersect_result> aa_cube::intersect_all(const ray &r) const
{
    vector3df p2 = p + size;
    double intersection[6] { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };
    if (r.direction.z < -eps || eps < r.direction.z)
    {
        intersection[0] = (p2.z - r.origin.z) / r.direction.z;
        intersection[1] = (p.z - r.origin.z) / r.direction.z;
    }
    if (r.direction.x < -eps || eps < r.direction.x)
    {
        intersection[2] = (p.x - r.origin.x) / r.direction.x;
        intersection[3] = (p2.x - r.origin.x) / r.direction.x;
    }
    if (r.direction.y < -eps || eps < r.direction.y)
    {
        intersection[4] = (p2.y - r.origin.y) / r.direction.y;
        intersection[5] = (p.y - r.origin.y) / r.direction.y;
    }

    std::vector<intersect_result> results;
    for (int i = 0; i < 6; ++i)
    {
        double &t = intersection[i];
        if (t > eps)
        {
            intersect_result ir(r.origin + r.direction * t, normals[i], t);
            if (((i == 0 || i == 1) &&
                 p.x - eps < ir.p.x && ir.p.x < p2.x + eps &&
                 p.y - eps < ir.p.y && ir.p.y < p2.y + eps) ||
                ((i == 2 || i == 3) &&
                 p.y - eps < ir.p.y && ir.p.y < p2.y + eps &&
                 p.z - eps < ir.p.z && ir.p.z < p2.z + eps) ||
                ((i == 4 || i == 5) &&
                 p.x - eps < ir.p.x && ir.p.x < p2.x + eps &&
                 p.z - eps < ir.p.z && ir.p.z < p2.z + eps))
            {
                results.push_back(ir);
            }
        }
    }

    // assert results.size() <= 2
    return results;
}