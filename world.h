#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <memory>

#include "object.h"

class world_intersect_result
{
public:
    bool succeeded = false;
    object &obj;
    vector3df p; // point
    vector3df n; // normal vector

    explicit world_intersect_result(bool succeeded) // Failed.
        : succeeded(succeeded), obj(object::dummy)
    {

    }

    world_intersect_result(object &obj, const vector3df &p, const vector3df &n)
        : succeeded(true), obj(obj), p(p), n(n)
    {

    }
};

class world
{
private:
    std::vector<std::shared_ptr<object> > _objects;

public:
    world()
    {

    }

    void add_object(const std::shared_ptr<object> &o_ptr)
    {
        _objects.push_back(o_ptr);
    }

    void add_object(std::shared_ptr<object> &&o_ptr)
    {
        _objects.push_back(std::move(o_ptr));
    }

    object &get_object(std::size_t i)
    {
        return *_objects[i];
    }

    // Note: references will be invalid.
    void del_object(std::size_t i)
    {
        _objects.erase(_objects.begin() + i);
    }

    std::vector<world_intersect_result> intersect_multi(const ray &r);
    world_intersect_result intersect(const ray &r);
};

#endif // _WORLD_H_