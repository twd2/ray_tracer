#include <cstddef>
#include <cstdio>

#include "camera.h"

vector3df camera::ray_trace(const ray &r) const
{
    world_intersect_result result = w.intersect(r);
    //printf("%d\n", result.succeeded);
    if (!result.succeeded)
    {
        return vector3df::zero;
    }

    double distance = (result.p - r.location).length();
    //printf("%lf\n", distance);
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
            const std::ptrdiff_t world_x = x, world_y = img.height - y - 1;
            const ray r = 
                ray(location,
                    vector3df((double)(world_x - half_width),
						      (double)(world_y - half_height),
						      (double)(-d)).normalize());
            vector3df color_float = ray_trace(r) * 255;
            img.set_color(x, y, color_t(color_float.x, color_float.y, color_float.z));
        }
    }
}