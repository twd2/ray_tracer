#ifndef _RAY_H_
#define _RAY_H_

#include <stack>

#include "vector3d.hpp"

class ray
{
public:
    vector3df origin, direction;
    double refractive_index; // origin refractive index

private:
    std::stack<double> _refractive_index_history;

public:
    // new ray
    ray(const vector3df &origin, const vector3df &direction)
        : origin(origin), direction(direction), refractive_index(1.0)
    {

    }

    // for reflection
    ray(const ray &r, const vector3df &origin, const vector3df &direction)
        : origin(origin), direction(direction), refractive_index(r.refractive_index),
          _refractive_index_history(r._refractive_index_history) // copy
    {

    }

    // for refraction
    ray(const ray &r, const vector3df &origin, const vector3df &direction,
        bool in_out, double new_refractive_index = 1.0)
        : origin(origin), direction(direction),
          _refractive_index_history(r._refractive_index_history)
    {
        if (in_out == in) // in
        {
            _refractive_index_history.push(r.refractive_index); // save old
            refractive_index = new_refractive_index; // load new
        }
        else if (in_out == out) // out
        {
            if (!_refractive_index_history.empty())
            {
                refractive_index = _refractive_index_history.top();
                _refractive_index_history.pop();
            }
            else
            {
                refractive_index = 1.0;
            }
        }
    }
    
    double last_refractive_index() const
    {
        if (!_refractive_index_history.empty())
        {
            return _refractive_index_history.top();
        }
        else
        {
            return 1.0;
        }
    }

    static constexpr bool in = true, out = false;
};

#endif // _RAY_H_