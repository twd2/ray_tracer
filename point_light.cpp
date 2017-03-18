#include "point_light.h"

#include "vector3d.hpp"
#include "light.h"

light_info point_light::illuminate(const vector3df &p) const
{
    vector3df direction = location - p;
    double distance = direction.length(); // save length
    direction = direction / distance; // normalize

    std::vector<world_intersect_result> results = w.intersect_all(ray(p, direction));
    vector3df coeff = vector3df(1.0, 1.0, 1.0);
    for (auto &ir : results)
    {
        if (ir.result.distance >= distance)
        {
            continue;
        }
        double distance_light2 = (ir.result.p - location).length2();
        if (distance_light2 < eps2)
        {
            continue;
        }

        if (ir.obj.transparency == vector3df::zero)
        {
            return light_info::dark;
        }

        coeff = coeff.modulate(ir.obj.transparency);
    }
    return light_info(color.modulate(coeff), -direction);
}
