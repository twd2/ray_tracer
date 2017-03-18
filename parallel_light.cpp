#include "parallel_light.h"

#include "vector3d.hpp"
#include "light.h"

light_info parallel_light::illuminate(const vector3df &p) const
{
    std::vector<world_intersect_result> results = w.intersect_all(ray(p, -direction));
    vector3df coeff = vector3df(1.0, 1.0, 1.0);
    for (auto &ir : results)
    {
        if (ir.obj.transparency == vector3df::zero)
        {
            return light_info::dark;
        }

        coeff = coeff.modulate(ir.obj.transparency);
    }
    return light_info(color.modulate(coeff), direction);
}
