#include "disc_light.h"

#include "vector3d.hpp"
#include "light.h"

light_info disc_light::illuminate(const vector3df &p) const
{
    vector3df direction = c - p;
    double distance = direction.length(); // save length
    direction = direction / distance; // normalize

    std::vector<world_intersect_result> results = w.intersect_all(ray(p, direction, 0, 0));
    vector3df coeff = vector3df::one;
    for (auto &ir : results)
    {
        if (ir.result.distance >= distance)
        {
            continue;
        }
        double distance_light2 = (ir.result.p - c).length2();
        if (distance_light2 < eps2)
        {
            continue;
        }

        if (ir.obj.refractiveness.length2() <= eps2)
        {
            return light_info::dark;
        }

        coeff = coeff.modulate(ir.obj.refractiveness);
    }
    return light_info(color.modulate(coeff), -direction);
}

ray disc_light::emit(std::default_random_engine &engine) const
{
    std::uniform_real_distribution<double> xy_dist(-r, r);

    // origin
    double x = xy_dist(engine), y = xy_dist(engine);
    while (x * x + y * y > r2)
    {
        x = xy_dist(engine);
        y = xy_dist(engine);
    }
    vector3df origin = c + xn * x + yn * y;

    // direction
    std::uniform_real_distribution<double> theta_dist(0.0, M_PI);
    std::uniform_real_distribution<double> phi_dist(0.0, 2 * M_PI);
    double theta = theta_dist(engine), phi = phi_dist(engine);
    vector3df dir = xn * (sin(theta) * cos(phi)) + yn * (sin(theta) * sin(phi)) + n * cos(theta);

    return ray(origin, dir, 0, 0);
}