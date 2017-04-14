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

mesh bezier_curve::to_rotate_surface_mesh(double dt, double dtheta) const
{
    std::vector<vector3df> points = to_points(dt);
    mesh result;
    dtheta *= 2.0 * M_PI / 360.0;
    std::size_t ntheta = 2.0 * M_PI / dtheta + 1;
    double theta = 0.0;
    std::size_t pid = 0;
    for (std::size_t i = 0; i < ntheta; ++i)
    {
        for (std::size_t j = 0; j < points.size(); ++j)
        {
            result.vertices.push_back(vector3df(points[j].x * cos(theta),
                                                points[j].y,
                                                -points[j].x * sin(theta)));
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

bezier_curve bezier_curve::load(const std::string &filename)
{
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