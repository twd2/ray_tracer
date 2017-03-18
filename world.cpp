#include <vector>
#include <memory>

#include "world.h"

const world_intersect_result world_intersect_result::failed(false);

std::vector<world_intersect_result> world::intersect_all(const ray &r)
{
    std::vector<world_intersect_result> results;
    for (auto &o_ptr : _objects)
    {
        std::vector<intersect_result> obj_results = o_ptr->intersect_all(r);
        for (auto &ir : obj_results)
        {
            results.push_back(world_intersect_result(*o_ptr, ir));
        }
    }
    return results;
}

world_intersect_result world::intersect(const ray &r)
{
    object *closest_obj = nullptr;
    intersect_result closest_result = intersect_result::failed;

    for (auto &o_ptr : _objects)
    {
        intersect_result ir = o_ptr->intersect(r);
        if (ir.succeeded)
        {
            if (!closest_obj || (closest_obj && ir.distance < closest_result.distance))
            {
                closest_obj = o_ptr.get();
                closest_result = ir;
            }
        }
    }

    if (closest_obj)
    {
        return world_intersect_result(*closest_obj, closest_result);
    }
    else
    {
        return world_intersect_result::failed;
    }
}