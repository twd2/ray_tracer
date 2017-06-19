#include <cinttypes>

#include "bezier_surface.h"

vector3df bezier_surface::get_point(double u, double v) const
{
    // de Casteljau's algorithm
    bezier_surface p = *this; // copy, k, l = 0
    // k = 1, ..., m, l = 0
    for (std::size_t k = 1; k < width; ++k)
    {
        for (std::size_t j = 0; j < height - 0; ++j)
        {
            for (std::size_t i = 0; i < width - k; ++i)
            {
                p(i, j) = p(i, j) * (1 - u) + p(i + 1, j) * u;
            }
        }
    }
    // k = m, l = 1, ..., n
    for (std::size_t l = 1; l < height; ++l)
    {
        for (std::size_t j = 0; j < height - l; ++j)
        {
            // for (std::size_t i = 0; i < width - (width - 1); ++i)
            p(0, j) = p(0, j) * (1 - v) + p(0, j + 1) * v;
        }
    }
    return p(0, 0);
}

mesh bezier_surface::to_mesh(double du, double dv) const
{
    mesh result;

    std::size_t nu = 1.0 / du + 1, nv = 1.0 / dv + 1;
    std::size_t pid = 0; // point index
    double u = 0.0, v = 0.0;
    for (std::size_t j = 0; j < nv; ++j)
    {
        u = 0.0;
        for (std::size_t i = 0; i < nu; ++i)
        {
            vector3df p = get_point(u, v);
            result.vertices.push_back(p);
            if (i > 0 && j > 0)
            {
                result.surfaces.push_back(vector3di(pid - 1, pid - nv - 1, pid - nv));
                result.surfaces.push_back(vector3di(pid - 1, pid - nv, pid));
            }
            ++pid;
            u += du;
        }
        v += dv;
    }
    return result;
}

bezier_surface bezier_surface::load(const std::string &filename)
{
    // TODO: more c++
    FILE *fd = fopen(filename.c_str(), "r");

    std::size_t width, height;
    fscanf(fd, "%" SCNu64 "%" SCNu64, &width, &height);
    bezier_surface result(width, height);
    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            vector3df &v = result(x, y);
            fscanf(fd, "%lf%lf%lf", &v.x, &v.y, &v.z);
        }
    }

    fclose(fd);

    return result;
}