#include "fog.h"

#include <cmath>
#include <random>

static std::default_random_engine engine;

intersect_result fog::intersect(const ray & r) const
{
    std::uniform_real_distribution<> dist_theta(0, M_PI);
    // TODO return intersect_result();
}
