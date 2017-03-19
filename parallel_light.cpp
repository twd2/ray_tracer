#include "parallel_light.h"

#include "vector3d.hpp"
#include "light.h"

light_info parallel_light::illuminate(const vector3df &p) const
{
    std::vector<world_intersect_result> results = w.intersect_all(ray(p, -direction));
    vector3df coeff = vector3df::one;
    for (auto &ir : results)
    {
        if (ir.obj.refractiveness.length2() <= eps2)
        {
            return light_info::dark;
        }

        coeff = coeff.modulate(ir.obj.refractiveness);
    }
    return light_info(color.modulate(coeff), direction);
}
