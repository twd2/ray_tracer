#include <cstddef>
#include <cstdio>
#include <random>

#include "camera.h"

#include "light.h"

static std::default_random_engine engine;

vector3df camera::ray_trace(const ray &r, const vector3df &contribution) const
{
    if (contribution.length2() < eps)
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
            Id += li.lightness.modulate(ir.obj.diffuse * N_dot_L);
        }

        vector3df R = -li.direction.reflect(ir.result.n);
        double R_dot_V = R.dot(r.direction);
        if (R_dot_V >= eps)
        {
            Is += li.lightness.modulate(ir.obj.specular * pow(R_dot_V, ir.obj.shininess));
        }
    }

    if (ir.obj.diffuse.length2() > eps2)
    {
        /*std::uniform_real_distribution<> dist(-1.0, 1.0);
        vector3df k = ir.result.n;
        vector3df i = k.cross(vector3df(dist(engine), dist(engine), dist(engine))).normalize();
        vector3df j = k.cross(i);
        const int diffuse_n = 1000;
        vector3df diffuseness = ir.obj.diffuse;// / diffuse_n;
        vector3df Id2 = vector3df::zero;
        for (int a = 0; a < diffuse_n; ++a)
        {
            vector3df v = i * dist(engine) + j * dist(engine) + k * ((dist(engine) + 1.0) / 2.0);
            double N_dot_V = v.dot(ir.result.n);
            vector3df coeff = diffuseness * N_dot_V;
            Id2 += ray_trace(ray(ir.result.p, v, r.refractive_index),
                                 vector3df::one * (100.0 * eps)).modulate(coeff);
        }
        Id += Id2; */
    }


    vector3df I = Id + Is;

    I = I * ((1 - ir.obj.reflectiveness) /** (1 - ir.obj.refractiveness)*/);

    vector3df reflectiveness = vector3df::one * ir.obj.reflectiveness;

    if (ir.obj.refractiveness.length2() > eps2)
    {
        double n_r = ir.obj.refractive_index;
        bool in_out = ray::in;
        if (ir.result.n.dot(r.direction) >= -eps) // out
        {
            in_out = ray::out;
            n_r = r.last_refractive_index();
        }

        const double n_i = r.refractive_index;
        double cosi, cosr;
        vector3df new_direction = r.direction.refract(ir.result.n, n_i, n_r,
                                                      cosi, cosr);
        if (new_direction != vector3df::zero)
        {
            double Rs = (n_i * cosi - n_r * cosr) / (n_i * cosi + n_r * cosr);
            Rs *= Rs;
            double Rp = (n_i * cosr - n_r * cosi) / (n_i * cosr + n_r * cosi);
            Rp *= Rp;
            double R = (Rs + Rp) / 2.0;
            double T = 1 - (Rs + Rp) / 2.0;

            vector3df refractiveness = ir.obj.refractiveness;
            refractiveness = refractiveness * T;
            reflectiveness = reflectiveness * R;

            vector3df Irefract =
                ray_trace(ray(r, ir.result.p, new_direction, in_out, n_r),
                    contribution.modulate(refractiveness)).modulate(refractiveness);
            I += Irefract;
        }
        else // total reflection
        {
            //reflectiveness = reflectiveness + ir.obj.refractiveness;
        }
    }

    if (reflectiveness.length2() > eps2)
    {
        vector3df Ireflect = ray_trace(ray(r, ir.result.p, r.direction.reflect(ir.result.n)),
            contribution.modulate(reflectiveness)).modulate(reflectiveness);
        I += Ireflect;
    }

    return I.capped();
}

void camera::render(image &img) const
{
    double aperture_samples2 = aperture_samples * aperture_samples;
    double delta = (double)aperture / aperture_samples;

    // TODO: Parallelization.
    std::ptrdiff_t half_width = img.width / 2, half_height = img.height / 2;
    for (std::ptrdiff_t y = 0; y < img.height; ++y)
    {
        for (std::ptrdiff_t x = 0; x < img.width; ++x)
        {
            const std::ptrdiff_t world_x = x, world_y = img.height - y - 1;
            vector3df color = vector3df::zero;
            const vector3df d = right * (double)(world_x - half_width) +
                                up * (double)(world_y - half_height) +
                                front * (double)(focal_length);
            if (aperture != 0.0)
            {
                const vector3df t = location + d;
                // samples
                vector3df o_y = location + up * (-aperture / 2.0) + right * (-aperture / 2.0);
                for (std::ptrdiff_t sample_y = 0; sample_y < aperture_samples; ++sample_y)
                {
                    vector3df o = o_y;
                    for (std::ptrdiff_t sample_x = 0; sample_x < aperture_samples; ++sample_x)
                    {
                        // o = location + right * (-aperture / 2.0 + sample_x * delta) +
                        //                up * (-aperture / 2.0 + sample_y * delta)
                        const ray r = ray(o, (t - o).normalize());
                        color += ray_trace(r, vector3df::one / aperture_samples2) /
                                 aperture_samples2;
                        o += right * delta;
                    }
                    o_y += up * delta;
                }
            }
            else // no depth of field
            {
                const ray r = ray(location, d.normalize());
                color = ray_trace(r, vector3df::one);
            }
            vector3df color_float = color * 255;
            img.set_color(x, y, color_t(color_float.x, color_float.y, color_float.z));
        }
        fprintf(stderr, "\rRendering... %5.2lf%%", (double)(y + 1) * 100.0 / img.height);
    }
    printf("\n");
}