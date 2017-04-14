#include "aa_box.h"

intersect_result aa_box::intersect(const ray &r) const
{
    return geometry.intersect(r);
}

std::vector<intersect_result> aa_box::intersect_all(const ray &r) const
{
    return geometry.intersect_all(r);
}