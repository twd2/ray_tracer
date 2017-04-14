#include "mesh.h"

#include <cstdio>
#include <cinttypes>

void mesh::save(const std::string &filename) const
{
    FILE *fd = fopen(filename.c_str(), "w");
    for (const auto &v : vertices)
    {
        fprintf(fd, "v %lf %lf %lf\n", v.x, v.y, v.z);
    }

    for (const auto &f : surfaces)
    {
        fprintf(fd, "f %" PRId64 " %" PRId64 " %" PRId64 "\n", f.x + 1, f.y + 1, f.z + 1);
    }

    fclose(fd);
}