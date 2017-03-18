#include <cstddef>
#include <cstdio>

#include "camera.h"

#include "light.h"

vector3df camera::ray_trace(const ray &r, const vector3df &contribution) const
{
    if (contribution.length2() <= eps)
    {
        return vector3df::zero;
    }

    world_intersect_result ir = w.intersect(r);
    if (!ir.succeeded)
    {
        return vector3df::zero;
    }

    vector3df Id = vector3df::zero;
    for (auto &light_ptr : w.lights)
    {
        light_info li = light_ptr->illuminate(ir.result.p);
        Id = Id + li.lightness.modulate(ir.obj.diffuse * abs(li.direction.dot(-ir.result.n)));
    }

    if (Id.x > 1.0)
    {
        Id.x = 1.0;
    }
    if (Id.y > 1.0)
    {
        Id.y = 1.0;
    }
    if (Id.z > 1.0)
    {
        Id.z = 1.0;
    }
    return Id;
    // return ir.result.n * 0.5 + vector3df(0.5, 0.5, 0.5);
    double distance = ir.result.distance;
    if (distance > 1000.0)
    {
        return vector3df::zero;
    }

    return vector3df(1.0, 1.0, 1.0) * (1000.0 - distance) / 1000.0;
}

void camera::render(image &img) const
{
    double d = img.width / double_tan_fov_x_2; // d = w / (2 * tan(fov / 2))

    // TODO: Parallelization.
    std::ptrdiff_t half_width = img.width / 2, half_height = img.height / 2;
    for (std::ptrdiff_t y = 0; y < img.height; ++y)
    {
        for (std::ptrdiff_t x = 0; x < img.width; ++x)
        {
            //if (x == 181 && y == 547)
            //{
            //    printf("here.");
            //}
            const std::ptrdiff_t world_x = x, world_y = img.height - y - 1;
            const ray r = 
                ray(location,
                    vector3df((double)(world_x - half_width),
                              (double)(world_y - half_height),
                              (double)(-d)).normalize());
            vector3df color_float = ray_trace(r, vector3df(1.0, 1.0, 1.0)) * 255;
            img.set_color(x, y, color_t(color_float.x, color_float.y, color_float.z));
        }
    }
}