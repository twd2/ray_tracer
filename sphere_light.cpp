#include "sphere_light.h"

#include "vector3d.hpp"
#include "light.h"

ray sphere_light::emit(std::default_random_engine &engine) const
{
    std::uniform_real_distribution<double> theta_dist(0.0, M_PI);
    std::uniform_real_distribution<double> phi_dist(0.0, 2 * M_PI);
    double theta = theta_dist(engine), phi = phi_dist(engine);
    vector3df dir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    return ray(location + dir * r, dir, 0, 0);
}