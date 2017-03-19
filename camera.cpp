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

    // Phong
    vector3df Id = vector3df::zero, Is = vector3df::zero;
    for (auto &light_ptr : w.lights)
    {
        light_info li = light_ptr->illuminate(ir.result.p);
        if (li.lightness == vector3df::zero)
        {
            continue;
        }

        double N_dot_L = li.direction.dot(-ir.result.n);
        if (N_dot_L >= eps)
        {
            Id = Id + li.lightness.modulate(ir.obj.diffuse * N_dot_L);
        }

        vector3df R = -li.direction.reflect(ir.result.n);
        double R_dot_V = R.dot(r.direction);
        if (R_dot_V >= eps)
        {
            Is = Is + li.lightness.modulate(ir.obj.specular * pow(R_dot_V, ir.obj.shininess));
        }
    }

    vector3df I = Id + Is;

    I = I * ((1 - ir.obj.reflectiveness) /** (1 - ir.obj.refractiveness)*/);

    if (ir.obj.reflectiveness > eps)
    {
        vector3df Ireflect = ray_trace(ray(ir.result.p, r.direction.reflect(ir.result.n), r.refractive_index), contribution * ir.obj.reflectiveness) * ir.obj.reflectiveness;
        I = I + Ireflect;
    }

    if (ir.obj.refractiveness.length2() > eps2)
    {
        double n_r = ir.obj.refractive_index;
        if (ir.result.n.dot(r.direction) >= eps)
        {
            n_r = 1.0;
        }
        vector3df Irefract =
            ray_trace(ray(ir.result.p, r.direction.refract(ir.result.n, r.refractive_index, n_r), n_r),
                      contribution.modulate(ir.obj.refractiveness)).modulate(ir.obj.refractiveness);
        I = I + Irefract;
    }

    return I.capped();
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