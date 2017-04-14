#include "mesh_object.h"

const mesh_object::triangle_intersect_result
mesh_object::triangle_intersect_result::failed(false);

mesh_object::mesh_object(const mesh &m)
    : object(), _mesh(m), _v(m.vertices), _tri(m.surfaces),
    _n(m.vertices.size()), _caches(m.surfaces.size())
{
    std::vector<triangle_index> kd_points;
    std::vector<vector_sum_count> sum_count(_mesh.vertices.size());
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

        // make normal vector and its count
        double area = cache.E1xE2.length() / 2.0; // = weight
        vector3df weighted_n = cache.n * area;
        sum_count[tri.x].count += area;
        sum_count[tri.x].sum += weighted_n;
        sum_count[tri.y].count += area;
        sum_count[tri.y].sum += weighted_n;
        sum_count[tri.z].count += area;
        sum_count[tri.z].sum += weighted_n;
    }

    // calc normal vectors of vertices
    for (std::size_t i = 0; i < _v.size(); ++i)
    {
        _n[i] = sum_count[i].sum / sum_count[i].count;
    }

    _kdt = kd_tree<triangle_index>::build(kd_points.begin(), kd_points.end());
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
        return intersect_result(r.origin + r.direction * result.t,
                                get_normal_vector(result), result.t);
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
        intersect_result ir(r.origin + r.direction * tir.t, get_normal_vector(tir), tir.t);
        result.push_back(ir);
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

    return triangle_intersect_result(i, t, 1 - (beta + gamma), beta, gamma);
}

vector3df mesh_object::get_normal_vector(const triangle_intersect_result &tir) const
{
    const vector3di &tri = _tri[tir.index];
    // normal vector interpolation
    const vector3df &n_a = _n[tri.x], &n_b = _n[tri.y], &n_c = _n[tri.z];
    vector3df n = n_a * tir.alpha + n_b * tir.beta + n_c * tir.gamma;
    n = n.normalize();
    //n = _caches[tir.index].n;
    return n;
}