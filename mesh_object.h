#ifndef _MESH_OBJECT_H_
#define _MESH_OBJECT_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"
#include "mesh.h"
#include "aa_cube.h"
#include "kd_tree.hpp"

class triangle_index;

class mesh_object
    : public object
{
private:
    struct triangle_cache
    {
        vector3df E1xE2, n;
    };

    class triangle_intersect_result
    {
    public:
        bool succeeded = false;
        std::size_t index;
        double t, alpha, beta, gamma;

        explicit triangle_intersect_result(bool succeeded) // Failed.
            : succeeded(succeeded)
        {

        }

        triangle_intersect_result(std::size_t index,
                                  double t, double alpha, double beta, double gamma)
            : succeeded(true), index(index), t(t), alpha(alpha), beta(beta), gamma(gamma)
        {

        }

        static const triangle_intersect_result failed;
    };

    const mesh _mesh;
    const std::vector<vector3df> &_v;
    const std::vector<vector3di> &_tri;
    std::vector<vector3df> _n; // normal vectors of vertices
    std::vector<triangle_cache> _caches; // params caches for triangle surfaces
    kd_tree<triangle_index> _kdt;

public:
    friend class triangle_index;

    mesh_object(const mesh &m);

    intersect_result intersect(const ray &r) const override;
    std::vector<intersect_result> intersect_all(const ray &r) const override;

private:
    triangle_intersect_result _intersect_triangle(const ray &r, std::size_t i) const;
    vector3df get_normal_vector(const triangle_intersect_result &tir) const;
    // unsigned int _intersect(const ray &r, kd_tree<triangle_index>::node *node) const;
    std::vector<triangle_intersect_result>
    _intersect_all(const ray &r, kd_tree<triangle_index>::node *node) const;
};

// triangle with index, for kd-tree
class triangle_index
{
private:
    const mesh_object *mo = nullptr;
    std::size_t i = 0;
    vector3df centre = vector3df::zero;

public:
    triangle_index() = default;

    triangle_index(const mesh_object &mo, std::size_t i)
        : mo(&mo), i(i),
          centre((mo._v[mo._tri[i].x] + mo._v[mo._tri[i].y] + mo._v[mo._tri[i].z]) / 3.0)
    {

    }

    double get_dim(std::size_t dim)
    {
        if (dim == 0)
        {
            return centre.x;
        }
        else if (dim == 1)
        {
            return centre.y;
        }
        else if (dim == 2)
        {
            return centre.z;
        }
        else
        {
            // ???
            return *reinterpret_cast<double *>(0);
        }
    }

    const double &get_dim(std::size_t dim) const
    {
        if (dim == 0)
        {
            return centre.x;
        }
        else if (dim == 1)
        {
            return centre.y;
        }
        else if (dim == 2)
        {
            return centre.z;
        }
        else
        {
            // ???
            return *reinterpret_cast<double *>(0);
        }
    }

    bool is_inside(const aa_cube &box) const
    {
        return box.is_inside(mo->_v[mo->_tri[i].x]) ||
               box.is_inside(mo->_v[mo->_tri[i].y]) ||
               box.is_inside(mo->_v[mo->_tri[i].z]);
    }

    bool not_inside(const aa_cube &box) const
    {
        return !box.is_inside(mo->_v[mo->_tri[i].x]) ||
               !box.is_inside(mo->_v[mo->_tri[i].y]) ||
               !box.is_inside(mo->_v[mo->_tri[i].z]);
    }
};

#endif // _MESH_OBJECT_H_