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
    bool smooth = true;

    mesh_object(const mesh &m);

    intersect_result intersect(const ray &r) const override;
    std::vector<intersect_result> intersect_all(const ray &r) const override;

private:
    triangle_intersect_result _intersect_triangle(const ray &r, std::size_t i) const;
    vector3df get_normal_vector(const triangle_intersect_result &tir) const;
    // unsigned int _intersect(const ray &r, kd_tree<triangle_index>::node *node) const;
    void _intersect_all(const ray &r, kd_tree<triangle_index>::node *node,
                        std::vector<triangle_intersect_result> &result) const;

    vector3df _texture_uv(const intersect_result &ir) const override
    {
        if (_mesh.texture.size() == 0)
        {
            return vector3df::zero;
        }

        double alpha = ir.u, beta = ir.v, gamma = 1.0 - (ir.u + ir.v);
        vector3df vta = _mesh.texture[_tri[ir.index].x],
                  vtb = _mesh.texture[_tri[ir.index].y],
                  vtc = _mesh.texture[_tri[ir.index].z];
        return vta * alpha + vtb * beta + vtc * gamma;
    }
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
        return centre.dim[dim];
    }

    const double &get_dim(std::size_t dim) const
    {
        return centre.dim[dim];
    }

    aa_cube get_aabb() const
    {
        // find axis-aligned bounding box
        vector3df min_v(mo->_v[mo->_tri[i].x].x, mo->_v[mo->_tri[i].x].y, mo->_v[mo->_tri[i].x].z);
        vector3df max_v = min_v;
        for (std::size_t vertex = 1; vertex < 3; ++vertex)
        {
            for (std::size_t dim = 0; dim < 3; ++dim)
            {
                if (mo->_v[mo->_tri[i].dim[vertex]].dim[dim] < min_v.dim[dim])
                {
                    min_v.dim[dim] = mo->_v[mo->_tri[i].dim[vertex]].dim[dim];
                }
                else if (mo->_v[mo->_tri[i].dim[vertex]].dim[dim] > max_v.dim[dim])
                {
                    max_v.dim[dim] = mo->_v[mo->_tri[i].dim[vertex]].dim[dim];
                }
            }
        }
        return aa_cube(min_v, max_v - min_v);
    }
};

#endif // _MESH_OBJECT_H_