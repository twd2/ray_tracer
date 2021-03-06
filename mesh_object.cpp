#include "mesh_object.h"

const mesh_object::triangle_intersect_result
mesh_object::triangle_intersect_result::failed(false);

mesh_object::mesh_object(const mesh &m)
    : object(), _mesh(m), _v(_mesh.vertices), _tri(_mesh.surfaces),
      _n(m.vertices.size()), _caches(m.surfaces.size())
{
    std::vector<triangle_index> kd_points;
    std::vector<double> count(_mesh.vertices.size());
    for (std::size_t i = 0; i < _tri.size(); ++i)
    {
        const vector3di &tri = _tri[i];
        const vector3df &a = _v[tri.x], &b = _v[tri.y], &c = _v[tri.z];

        kd_points.push_back(triangle_index(*this, i));
    
        // make cache
        triangle_cache cache;
        vector3df E1 = a - b;
        vector3df E2 = a - c;
        cache.E1xE2 = E1.cross(E2);
        cache.n = cache.E1xE2.normalize();
        _caches[i] = cache;

        if (m.normals.size() == 0)
        {
            // make normal vector and its count
            double area = cache.E1xE2.length() / 2.0; // = weight
            if (area > eps)
            {
                vector3df weighted_n = cache.n * area;
                count[tri.x] += area;
                _n[tri.x] += weighted_n;
                count[tri.y] += area;
                _n[tri.y] += weighted_n;
                count[tri.z] += area;
                _n[tri.z] += weighted_n;
            }
        }
    }

    if (m.normals.size() == 0)
    {
        // calc normal vectors of vertices
        for (std::size_t i = 0; i < _v.size(); ++i)
        {
            _n[i] = (_n[i] / count[i]).normalize();
        }
    }
    else
    {
        _n = m.normals; // just copy
    }

    printf("Building kd-tree (mesh)...\n");
    _kdt = kd_tree<triangle_index>::build(kd_points.begin(), kd_points.end(), true);
}

intersect_result mesh_object::intersect(const ray &r) const
{
    // TODO: performance improve
    std::vector<intersect_result> results = intersect_all(r);
    intersect_result *closest_result = nullptr;

    for (intersect_result &ir : results)
    {
        if (!closest_result || ir.distance < closest_result->distance)
        {
            closest_result = &ir;
        }
    }

    if (closest_result)
    {
        return *closest_result;
    }
    else
    {
        return intersect_result::failed;
    }
}

std::vector<intersect_result> mesh_object::intersect_all(const ray &r) const
{
    // deduplicate
    std::vector<triangle_intersect_result> tirs;
    _intersect_all(r, _kdt.root.get(), tirs);
    std::vector<bool> added(_tri.size());
    for (std::size_t i = 0; i < _tri.size(); ++i)
    {
        added[i] = false;
    }

    std::vector<intersect_result> result;
    result.reserve(tirs.size());
    for (const auto &tir : tirs)
    {
        if (!added[tir.index])
        {
            intersect_result ir(r.origin + r.direction * tir.t, get_normal_vector(tir), tir.t,
                                tir.alpha, tir.beta, tir.index);
            result.push_back(ir);
            added[tir.index] = true;
        }
    }

    return result;
}

mesh_object::triangle_intersect_result
mesh_object::_intersect_triangle(const ray &r, std::size_t i) const
{
    const vector3di &tri = _tri[i];
    const vector3df &a = _v[tri.x], &b = _v[tri.y], &c = _v[tri.z];
    const triangle_cache &cache = _caches[i];

    vector3df E1 = a - b;
    vector3df E2 = a - c;

    double divisor = cache.E1xE2.dot(r.direction);
    if (divisor <= eps && divisor >= -eps)
    {
        return triangle_intersect_result::failed;
    }
    double divisor_inv = 1.0 / divisor;

    vector3df S = a - r.origin;
    double t = cache.E1xE2.dot(S) * divisor_inv;
    if (t <= eps)
    {
        return triangle_intersect_result::failed;
    }
    vector3df DxS = r.direction.cross(S);
    double beta = DxS.dot(E2) * divisor_inv;
    if (beta <= eps || beta > 1.0)
    {
        return triangle_intersect_result::failed;
    }

    double gamma = DxS.dot(-E1) * divisor_inv;

    if (gamma <= eps || (beta + gamma) > 1.0)
    {
        return triangle_intersect_result::failed;
    }

    return triangle_intersect_result(i, t, 1.0 - (beta + gamma), beta, gamma);
}

vector3df mesh_object::get_normal_vector(const triangle_intersect_result &tir) const
{
    const vector3di &tri = _tri[tir.index];
    if (smooth)
    {
        // normal vector interpolation
        const vector3df &n_a = _n[tri.x], &n_b = _n[tri.y], &n_c = _n[tri.z];
        vector3df n = n_a * tir.alpha + n_b * tir.beta + n_c * tir.gamma;
        n = n.normalize();
        return n;
    }
    else
    {
        return _caches[tir.index].n;
    }
}

void mesh_object::_intersect_all(const ray &r, kd_tree<triangle_index>::node *node,
                                 std::vector<triangle_intersect_result> &result) const
{
    if (!node || !node->range.intersect(r).succeeded)
    {
        return;
    }
    if (node->left && node->right)
    {
        _intersect_all(r, node->left, result);
        _intersect_all(r, node->right, result);
    }
    else if (node->left)
    {
        _intersect_all(r, node->left, result);
    }
    else if (node->right)
    {
        _intersect_all(r, node->right, result);
    }
    else
    {
        for (std::size_t i = 0; i < node->size; ++i)
        {
            triangle_intersect_result tir = _intersect_triangle(r, node->points[i]);
            if (!tir.succeeded)
            {
                continue;
            }
            result.push_back(tir);
        }
    }
}