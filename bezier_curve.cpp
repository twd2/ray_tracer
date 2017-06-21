#include "bezier_curve.h"

#include <cmath>
#include <cinttypes>

vector3df bezier_curve::get_point(double t) const
{
    // de Casteljau's algorithm
    bezier_curve p = *this; // copy, k = 0
    for (std::size_t k = 1; k <= n; ++k)
    {
        for (std::size_t i = 0; i <= n - k; ++i)
        {
            p[i] = p[i] * (1 - t) + p[i + 1] * t;
        }
    }
    return p[0];
}

vector3df bezier_curve::d_dt(double t) const
{
    if (!_cache)
    {
        _cache = std::unique_ptr<bezier_curve>(new bezier_curve(n - 1));
        for (std::size_t i = 0; i <= n - 1; ++i)
        {
            _cache->data[i] = (data[i + 1] - data[i]) * n;
        }
    }

    return _cache->get_point(t);
}

std::vector<vector3df> bezier_curve::to_points(double dt) const
{
    std::vector<vector3df> result;
    std::size_t nt = 1.0 / dt + 1;
    double t = 0.0;
    for (std::size_t i = 0; i < nt; ++i)
    {
        result.push_back(get_point(t));
        t += dt;
    }
    return result;
}

std::vector<vector3df> bezier_curve::to_tangents(double dt) const
{
    std::vector<vector3df> result;
    std::size_t nt = 1.0 / dt + 1;
    double t = 0.0;
    for (std::size_t i = 0; i < nt; ++i)
    {
        result.push_back(d_dt(t));
        t += dt;
    }
    return result;
}

mesh bezier_curve::to_rotate_surface_mesh(double dt, double dtheta) const
{
    std::vector<vector3df> points = to_points(dt),
                           tangents = to_tangents(dt);

    mesh result;
    dtheta *= 2.0 * M_PI / 360.0;
    std::size_t ntheta = 2.0 * M_PI / dtheta + 1;
    double theta = 0.0;
    std::size_t pid = 0;
    for (std::size_t i = 0; i < ntheta; ++i)
    {
        for (std::size_t j = 0; j < points.size(); ++j)
        {
            double cos_theta = cos(theta), sin_theta = sin(theta);
            vector3df ddt(tangents[j].x * cos_theta,
                          tangents[j].y,
                          -tangents[j].x * sin_theta),
                      ddtheta(points[j].x * -sin_theta,
                              0.0,
                              -points[j].x * cos_theta),
                      p(points[j].x * cos_theta,
                        points[j].y,
                        -points[j].x * sin_theta);
            vector3df norm;
            if (ddtheta.length2() < eps2)
            {
                norm = vector3df::up;
            }
            else
            {
                norm = -ddt.cross(ddtheta).normalize();
            }
            result.vertices.push_back(p);
            //result.normals.push_back(norm); // TODO
            result.texture.push_back(vector3df(theta / (2 * M_PI),
                                     (double)j / (double)(points.size() - 1),
                                     0.0));

            if (i > 0 && j > 0)
            {
                result.surfaces.push_back(vector3di(pid, pid - points.size() - 1, pid - 1));
                result.surfaces.push_back(vector3di(pid, pid - points.size(), pid - points.size() - 1));
            }
            ++pid;
        }
        theta += dtheta;
    }
    for (std::size_t j = 1; j < points.size(); ++j)
    {
        result.surfaces.push_back(vector3di(j, pid - points.size() + j - 1, j - 1));
        result.surfaces.push_back(vector3di(j, pid - points.size() + j, pid - points.size() + j - 1));
    }
    return result;
}

vector3df bezier_curve::d_dt(double t, double theta) const
{
    vector3df tangent = d_dt(t);
    return vector3df(tangent.x * cos(theta),
                     tangent.y,
                     -tangent.x * sin(theta));
}

vector3df bezier_curve::d_dtheta(double t, double theta) const
{
    vector3df p = get_point(t);
    return vector3df(p.x * -sin(theta),
                     0.0,
                     -p.x * cos(theta));
}

void bezier_curve::get(double t, double theta, vector3df &out_point,
                       vector3df &out_d_dt, vector3df &out_d_dtheta) const
{
    double cos_theta = cos(theta), sin_theta = sin(theta);
    vector3df tangent = d_dt(t);
    out_d_dt = vector3df(tangent.x * cos_theta,
                         tangent.y,
                         -tangent.x * sin_theta);
    vector3df p = get_point(t);
    out_d_dtheta = vector3df(p.x * -sin_theta,
                             0.0,
                             -p.x * cos_theta);
    out_point = vector3df(p.x * cos_theta,
                          p.y,
                          -p.x * sin_theta);
}

intersect_result bezier_curve::intersect(const ray &r, double t0, double u0, double v0) const
{
    // u: t, v: theta
    double t = t0, u = u0, v = v0;
    vector3df point, d_dt, d_dtheta;
    for (std::size_t i = 0; ; ++i)
    {
        get(u, v, point, d_dt, d_dtheta);
        vector3df f = r.origin + r.direction * t - point;
        if (f.length2() < eps2)
        {
            break;
        }

        double D = r.direction.dot(d_dt.cross(d_dtheta));
        t -= d_dt.dot(d_dtheta.cross(f)) / D;
        u -= r.direction.dot(d_dtheta.cross(f)) / D;
        v += r.direction.dot(d_dt.cross(f)) / D;
        if (v < 0.0)
        {
            v = -v + M_PI;
        }
        if (v >= 2 * M_PI)
        {
            v = fmod(v, 2 * M_PI);
        }
        printf("i%llu: t=%0.10lf, u=%0.10lf, v=%0.10lf, v/pi=%0.10lf\n", i, t, u, v, v / M_PI);
    }
    return intersect_result::failed;
}

bezier_curve bezier_curve::load(const std::string &filename)
{
    // TODO: more c++
    FILE *fd = fopen(filename.c_str(), "r");

    std::size_t n;
    fscanf(fd, "%" SCNu64, &n);
    bezier_curve result(n);
    for (std::size_t i = 0; i <= n; ++i)
    {
        vector3df &v = result[i];
        fscanf(fd, "%lf%lf%lf", &v.x, &v.y, &v.z);
    }

    fclose(fd);

    return result;
}