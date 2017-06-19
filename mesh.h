#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <string>

#include "vector3d.hpp"

class mesh
{
public:
    std::vector<vector3df> vertices;
    std::vector<vector3df> normals; // normals for vertices
    std::vector<vector3di> surfaces; // triangles, stores index of vertices, starts at 0

    void save(const std::string &filename) const;
    static mesh load(const std::string &filename); // TODO
};

#endif // _MESH_H_