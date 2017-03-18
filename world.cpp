#include <vector>
#include <memory>

#include "world.h"

const world_intersect_result world_intersect_result::failed(false);

std::vector<world_intersect_result> world::intersect_multi(const ray &r)
{
    std::vector<world_intersect_result> results;
    for (auto &o_ptr : _objects)
    {
        intersect_result ir = o_ptr->intersect(r);
        if (ir.succeeded)
        {
            results.push_back(world_intersect_result(*o_ptr, ir));
        }
    }
    return results;
}

world_intersect_result world::intersect(const ray &r)
{
    std::vector<world_intersect_result> results = intersect_multi(r);
    if (results.size() == 0)
    {
        return world_intersect_result::failed;
    }

    if (results.size() == 1)
    {
        return results[0];
    }

    world_intersect_result *result_ptr = &results[0];
    double min_distance = results[0].result.distance;

    for (std::size_t i = 1; i < results.size(); ++i)
    {
        double distance = results[i].result.distance;
        if (distance < min_distance)
        {
            min_distance = distance;
            result_ptr = &results[i];
        }
    }

    return *result_ptr;
}