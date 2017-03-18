#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <memory>

#include "object.h"
#include "light.h"

class world_intersect_result
{
public:
    bool succeeded = false;
    object &obj;
    intersect_result result;

    explicit world_intersect_result(bool succeeded) // Failed.
        : succeeded(succeeded), obj(object::dummy), result(intersect_result::failed)
    {

    }

    world_intersect_result(object &obj, const intersect_result &result)
        : succeeded(true), obj(obj), result(result)
    {

    }

    static const world_intersect_result failed;
};

class light;

class world
{
private:
    std::vector<std::shared_ptr<object> > _objects;

public:
    std::vector<std::shared_ptr<light> > lights;

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

    std::vector<world_intersect_result> intersect_all(const ray &r);
    world_intersect_result intersect(const ray &r);
};

#endif // _WORLD_H_