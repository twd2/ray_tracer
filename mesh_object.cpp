#include "mesh_object.h"

const mesh_object::triangle_intersect_result
mesh_object::triangle_intersect_result::failed(false);

mesh_object::mesh_object(const mesh &m)
    : object(), _mesh(m), _v(m.vertices), _tri(m.surfaces),
    _n(m.vertices.size()), _caches(m.surfaces.size())
{
    typedef std::pair<double, vector3df> weighted_n_t;

    // TODO: build kd-tree

    std::vector<std::vector<weighted_n_t> > weighted_ns(_mesh.vertices.size());
    for (std::size_t i = 0; i < _tri.size(); ++i)
    {
        const vector3di &tri = _tri[i];
        const vector3df &a = _v[tri.x], &b = _v[tri.y], &c = _v[tri.z];
    
        // make cache
        triangle_cache cache;
        cache.E1 = a - b;
        cache.E2 = a - c;
        cache.E1xE2 = cache.E1.cross(cache.E2);
        cache.n = cache.E1xE2.normalize();
        _caches[i] = cache;

        // make normal vector and its weight
        double area = cache.E1xE2.length() / 2.0; // = weight
        weighted_n_t weighted_n = std::make_pair(area, cache.n);
        weighted_ns[tri.x].push_back(weighted_n);
        weighted_ns[tri.y].push_back(weighted_n);
        weighted_ns[tri.z].push_back(weighted_n);
    }

    // calc normal vectors of vertices
    for (std::size_t i = 0; i < _v.size(); ++i)
    {
        vector3df n = vector3df::zero;
        double total_weight = 0.0;
        for (const auto &weighted_n : weighted_ns[i])
        {
            n += weighted_n.second; // n
            total_weight += weighted_n.first; // weight
        }
        _n[i] = n / total_weight;
    }
}

intersect_result mesh_object::intersect(const ray &r) const
{
    // TODO: use kd-tree
    triangle_intersect_result result = triangle_intersect_result::failed;
    for (std::size_t i = 0; i < _tri.size(); ++i)
    {
        triangle_intersect_result tir = _intersect_triangle(r, i);
        if (!tir.succeeded)
        {
            continue;
        }
        if (!result.succeeded || tir.t < result.t)
        {
            result = tir;
        }
    }

    if (!result.succeeded)
    {
        return intersect_result::failed;
    }
    else
    {
        const vector3di &tri = _tri[result.index];
        // normal vector interpolation
        const vector3df &n_a = _n[tri.x], &n_b = _n[tri.y], &n_c = _n[tri.z];
        vector3df n = n_a * result.alpha + n_b * result.beta + n_c * result.gamma;
        n = n.normalize();
        n = _caches[result.index].n;
        return intersect_result(r.origin + r.direction * result.t, n, result.t);
    }
}

std::vector<intersect_result> mesh_object::intersect_all(const ray &r) const
{
    std::vector<intersect_result> result;
    // TODO: use kd-tree
    for (std::size_t i = 0; i < _tri.size(); ++i)
    {
        triangle_intersect_result tir = _intersect_triangle(r, i);
        if (!tir.succeeded)
        {
            continue;
        }
        const vector3di &tri = _tri[i];
        // normal vector interpolation
        const vector3df &n_a = _n[tri.x], &n_b = _n[tri.y], &n_c = _n[tri.z];
        vector3df n = n_a * tir.alpha + n_b * tir.beta + n_c * tir.gamma;
        n = n.normalize();
        n = _caches[i].n;
        intersect_result ir(r.origin + r.direction * tir.t, n, tir.t);
        result.push_back(ir);
    }

    return result;
}

mesh_object::triangle_intersect_result
mesh_object::_intersect_triangle(const ray &r, std::size_t i) const
{
    const vector3di &tri = _tri[i];
    const vector3df &a = _v[tri.x];
    const triangle_cache &cache = _caches[i];

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
    double beta = DxS.dot(cache.E2) * divisor_inv;
    if (beta <= eps || beta > 1.0)
    {
        return triangle_intersect_result::failed;
    }

    double gamma = DxS.dot(-cache.E1) * divisor_inv;

    if (gamma <= eps || (beta + gamma) > 1.0)
    {
        return triangle_intersect_result::failed;
    }

    return triangle_intersect_result(i, t, 1 - (beta + gamma), beta, gamma);
}