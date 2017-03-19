#include "fog.h"

#include <cmath>
#include <random>

static std::default_random_engine engine;

intersect_result fog::intersect(const ray &r) const
{
    std::vector<intersect_result> results = boundary.intersect_all(r);
    if (results.size() == 0)
    {
        return intersect_result::failed;
    }

    std::uniform_real_distribution<> dist(0.0, 1.0);
    if (dist(engine) < 0.8)
    {
        return intersect_result::failed;
    }

    double left, right;

    if (results.size() == 1)
    {
        left = 0;
        right = results[0].distance;
    }

    if (results.size() == 2)
    {
        left = results[0].distance;
        right = results[1].distance;
        if (right < left)
        {
            std::swap(left, right);
        }
    }

    std::uniform_real_distribution<> dist_t(left, right);

    /*std::uniform_real_distribution<> dist_theta(0.0, M_PI);
    std::uniform_real_distribution<> dist_phi(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<> dist_radius(0.0, boundary.r);
    double theta = dist_theta(engine), phi = dist_phi(engine), radius = dist_radius(engine);
    vector3df p = vector3df(radius * cos(theta) * cos(phi),
                            radius * cos(theta) * sin(phi),
                            radius * sin(theta));*/
    vector3df n = vector3df(dist(engine), dist(engine), dist(engine)).normalize();
    double t = dist_t(engine);
    return intersect_result(r.origin + r.direction * t, n, t);
}
