#include <cstddef>
#include <cstdio>
#include <ctime>
#include <random>
#include <thread>

#include "camera.h"

#include "light.h"

static std::default_random_engine engine(time(nullptr));

vector3df camera::ray_trace(const ray &r, const vector3df &contribution)
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

    if (ir.obj.diffuse.length2() > eps2)
    {
        hit_point hp(r, ir.obj, ir.result);
        hp.contribution = contribution * (1 - ir.obj.reflectiveness);

        {
            std::unique_lock<std::mutex> lock(_hit_points_lock);
            _hit_points.push_back(hp);
        }
    }

    vector3df I = vector3df::zero;
    I += ir.obj.emission;
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

        }
    }

    if (reflectiveness.length2() > eps2)
    {
        vector3df Ireflect = ray_trace(ray(r, ir.result.p, r.direction.reflect(ir.result.n)),
            contribution.modulate(reflectiveness)).modulate(reflectiveness);
        I += Ireflect;
    }

    return I;
}

void camera::photon_trace(const ray &r, const vector3df &contribution, double radius,
                          std::size_t depth)
{
    if (depth > diffuse_depth)
    {
        return;
    }

    if (contribution.length2() < eps)
    {
        return;
    }

    world_intersect_result ir = w.intersect(r);
    if (!ir.succeeded)
    {
        return;
    }

    if (ir.obj.diffuse.length2() > eps2)
    {
        std::vector<unsigned int> hit_point_ids =
            _hit_point_inside(sphere(ir.result.p, radius));
        for (const auto &i : hit_point_ids)
        {
            hit_point &hp = _hit_points[i];
            if ((ir.result.p - hp.p).length2() > hp.radius2)
            {
                continue;
            }

            vector3df flux = hp.obj->brdf(_to_intersect_result(hp),
                                          hp.ray_direction,
                                          r.direction).modulate(contribution);

            {
                std::unique_lock<std::mutex> lock(_hit_points_lock);
                ++hp.new_photon_count;
                hp.flux += flux;
            }
        }

        // diffuse
        vector3df z = ir.result.n;
        vector3df x = z.get_vert();
        vector3df y = z.cross(x);
        std::uniform_real_distribution<double> theta_dist(0.0, M_PI / 2);
        std::uniform_real_distribution<double> phi_dist(0.0, 2 * M_PI);
        double theta = theta_dist(engine), phi = phi_dist(engine);
        vector3df dir = x * (sin(theta) * cos(phi)) + y * (sin(theta) * sin(phi)) + z * cos(theta);
        photon_trace(ray(r, ir.result.p, dir),
                     contribution.modulate(ir.obj.get_diffuse(ir.result)),
                     radius, depth + 1);
    }

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

            photon_trace(ray(r, ir.result.p, new_direction, in_out, n_r),
                         contribution.modulate(refractiveness), radius, depth);
        }
        else // total reflection
        {

        }
    }

    if (reflectiveness.length2() > eps2)
    {
        photon_trace(ray(r, ir.result.p, r.direction.reflect(ir.result.n)),
                     contribution.modulate(reflectiveness), radius, depth);
    }
}

void camera::ray_trace_pass(imagef &img)
{
    _hit_points.clear();

    double aperture_samples2 = aperture_samples * aperture_samples;
    double delta = (double)aperture / aperture_samples;

    std::ptrdiff_t progress = 0;
    double half_width = (double)film_width / 2.0, half_height = (double)film_height / 2.0;
    auto task = [&] (std::ptrdiff_t begin, std::ptrdiff_t end, bool print_progress)
    {
        for (std::ptrdiff_t y = begin; y < end; ++y)
        {
            for (std::ptrdiff_t x = 0; x < img.width; ++x)
            {
                const double world_x = (double)x * film_width / img.width,
                             world_y = (double)(img.height - y - 1) * film_height / img.height;
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
                            const ray r = ray(o, (t - o).normalize(), x, y);
                            color += ray_trace(r, vector3df::one / aperture_samples2) /
                                     aperture_samples2;
                            o += right * delta;
                        }
                        o_y += up * delta;
                    }
                }
                else // no depth of field
                {
                    const ray r = ray(location, d.normalize(), x, y);
                    color = ray_trace(r, vector3df::one);
                }
                img(x, y) = color.capped();
            }
            ++progress;
            if (print_progress)
            {
                fprintf(stderr, "\rRay tracing... %5.2lf%%", (double)progress * 100.0 / img.height);
            }
        }
    };

    std::vector<std::shared_ptr<std::thread> > tasks;
    std::size_t chunk_size = img.height / thread_count;
    for (std::size_t i = 0; i < thread_count - 1; ++i)
    {
        tasks.push_back(std::make_shared<std::thread>(task, i * chunk_size, (i + 1) * chunk_size, true));
    }
    task(chunk_size * (thread_count - 1), img.height, true);
    for (std::size_t i = 0; i < thread_count - 1; ++i)
    {
        tasks[i]->join();
    }
    
    fprintf(stderr, "\n");
    printf("%lu hit points\n", _hit_points.size());
}

double camera::photon_trace_pass(int photon_count, double radius)
{
    constexpr double alpha = 0.7;

    bool is_first_pass = false;

    if (!_kdt.root)
    {
        printf("Building kd-tree (hit points)...\n");
        _kdt = kd_tree<hit_point>::build(_hit_points.begin(), _hit_points.end(), true);
        for (auto &hp : _hit_points)
        {
            hp.radius2 = radius * radius;
        }
        is_first_pass = true;
    }

    // emit rays
    std::size_t progress = 0;
    auto task = [&] (std::size_t begin, std::size_t end, bool print_progress)
    {
        for (std::size_t i = begin; i < end; ++i)
        {
            // choose a light
            std::uniform_int_distribution<std::size_t> dist(0, w.lights.size() - 1);
            light &l = *w.lights[dist(engine)];
            ray r = l.emit(engine);
            photon_trace(r, l.flux(), radius);
            ++progress;
            if (print_progress && (progress & 1023) == 0)
            {
                fprintf(stderr, "\rPhoton tracing... %5.2lf%%",
                        (double)progress * 100.0 / photon_count);
            }
        }
    };

    std::vector<std::shared_ptr<std::thread> > tasks;
    std::size_t chunk_size = photon_count / thread_count;
    for (std::size_t i = 0; i < thread_count - 1; ++i)
    {
        tasks.push_back(std::make_shared<std::thread>(task, i * chunk_size, (i + 1) * chunk_size, true));
    }
    task(chunk_size * (thread_count - 1), photon_count, true);
    for (std::size_t i = 0; i < thread_count - 1; ++i)
    {
        tasks[i]->join();
    }

    fprintf(stderr, "\n");

    if (!is_first_pass)
    {
        double max_radius2 = 0.0, min_radius2 = 1e6;
        for (auto &hp : _hit_points)
        {
            double coeff = (hp.photon_count + alpha * hp.new_photon_count) /
                           (hp.photon_count + hp.new_photon_count);
            if (hp.photon_count + hp.new_photon_count == 0)
            {
                coeff = 1.0;
            }
            hp.radius2 *= coeff;
            if (hp.radius2 > max_radius2)
            {
                max_radius2 = hp.radius2;
            }
            else if (hp.radius2 < min_radius2)
            {
                min_radius2 = hp.radius2;
            }
            hp.flux = hp.flux * coeff;
            hp.photon_count += alpha * hp.new_photon_count;
            hp.new_photon_count = 0;
        }
        printf("max radius %lf\n", sqrt(max_radius2));
        printf("min radius %lf\n", sqrt(min_radius2));
        return sqrt(max_radius2);
    }
    else
    {
        for (auto &hp : _hit_points)
        {
            hp.photon_count = hp.new_photon_count;
            hp.new_photon_count = 0;
        }
        return radius;
    }
}

void camera::phong_estimate(imagef &img)
{
    std::size_t progress = 0;
    auto task = [&](std::size_t begin, std::size_t end, bool print_progress)
    {
        for (std::size_t i = begin; i < end; ++i)
        {
            auto &hp = _hit_points[i];
            vector3df Id = vector3df::zero, Is = vector3df::zero;
            for (auto &light_ptr : w.lights)
            {
                light_info li = light_ptr->illuminate(hp.p);
                if (li.lightness == vector3df::zero)
                {
                    continue;
                }

                double N_dot_L = li.direction.dot(-hp.n);
                if (N_dot_L >= eps)
                {
                    Id += li.lightness.modulate(hp.obj->get_diffuse(_to_intersect_result(hp)) *
                                                N_dot_L); // TODO: texture
                }

                vector3df R = -li.direction.reflect(hp.n);
                double R_dot_V = R.dot(hp.ray_direction);
                if (R_dot_V >= eps)
                {
                    Is += li.lightness.modulate(hp.obj->specular * pow(R_dot_V, hp.obj->shininess));
                }
            }

            vector3df I = Id + Is;
            I = I.modulate(hp.contribution);

            {
                std::unique_lock<std::mutex> lock(_hit_points_lock);
                img(hp.image_x, hp.image_y) = (img(hp.image_x, hp.image_y) + I).capped();
            }

            ++progress;
            if (print_progress && (progress & 1023) == 0)
            {
                fprintf(stderr, "\rEstimating diffuse using Phone model... %5.2lf%%",
                        (double)progress * 100.0 / _hit_points.size());
            }
        }
    };

    std::vector<std::shared_ptr<std::thread> > tasks;
    std::size_t chunk_size = _hit_points.size() / thread_count;
    for (std::size_t i = 0; i < thread_count - 1; ++i)
    {
        tasks.push_back(std::make_shared<std::thread>(task, i * chunk_size, (i + 1) * chunk_size, true));
    }
    task(chunk_size * (thread_count - 1), _hit_points.size(), true);
    for (std::size_t i = 0; i < thread_count - 1; ++i)
    {
        tasks[i]->join();
    }

    fprintf(stderr, "\n");
}

void camera::ppm_estimate(imagef &img, int photon_count)
{
    if (!photon_count)
    {
        return;
    }

    for (std::size_t i = 0; i < _hit_points.size(); ++i)
    {
        auto &hp = _hit_points[i];
        
        vector3df I = hp.flux / (M_PI * hp.radius2 * photon_count);
        I = I.modulate(hp.contribution);
        img(hp.image_x, hp.image_y) = (img(hp.image_x, hp.image_y) + I).capped();
        if ((i & 1023) == 0)
        {
            fprintf(stderr, "\rEstimating diffuse using PPM... %5.2lf%%",
                    (double)(i + 1) * 100.0 / _hit_points.size());
        }
    }
    fprintf(stderr, "\n");
}

std::vector<unsigned int> camera::_hit_point_inside(const sphere &r) const
{
    // deduplicate
    std::vector<unsigned int> result;
    _hit_point_inside(r, _kdt.root.get(), result);
    std::vector<bool> added(_hit_points.size());
    for (std::size_t i = 0; i < _hit_points.size(); ++i)
    {
        added[i] = false;
    }

    std::vector<unsigned int> result_deduplicated;
    result_deduplicated.reserve(result.size());
    for (const auto &i : result)
    {
        if (!added[i])
        {
            result_deduplicated.push_back(i);
            added[i] = true;
        }
    }

    return result_deduplicated;
}

void camera::_hit_point_inside(const sphere &r, kd_tree<hit_point>::node *node,
                               std::vector<unsigned int> &result) const
{
    if (!node)
    {
        return;
    }

    vector3df delta = vector3df::one * r.r;
    aa_cube big_cube(node->range.p - delta, node->range.size + delta * 2);

    if (!big_cube.is_inside(r.c))
    {
        return;
    }

    if (node->left && node->right)
    {
        _hit_point_inside(r, node->left, result);
        _hit_point_inside(r, node->right, result);
    }
    else if (node->left)
    {
        _hit_point_inside(r, node->left, result);
    }
    else if (node->right)
    {
        _hit_point_inside(r, node->right, result);
    }
    else
    {
        for (std::size_t i = 0; i < node->size; ++i)
        {
            const hit_point &hp = _hit_points[node->points[i]];
            if ((hp.p - r.c).length2() < r.r2)
            {
                result.push_back(node->points[i]);
            }
        }
    }
}