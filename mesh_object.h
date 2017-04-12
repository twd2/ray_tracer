#ifndef _MESH_OBJECT_H_
#define _MESH_OBJECT_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"
#include "mesh.h"

class mesh_object
    : public object
{
private:
    struct triangle_cache
    {
        vector3df E1, E2, E1xE2, n;
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

public:
    mesh_object(const mesh &m);

    intersect_result intersect(const ray &r) const override;
    std::vector<intersect_result> intersect_all(const ray &r) const override;

private:
    triangle_intersect_result _intersect_triangle(const ray &r, std::size_t i) const;
};


#endif // _MESH_OBJECT_H_