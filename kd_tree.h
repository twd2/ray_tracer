#ifndef _KD_TREE_HPP_
#define _KD_TREE_HPP_

#include <memory>
#include <vector>
#include <iterator>
#include <algorithm>

#include "aa_cube.h"

// T should have T::get_dim(i), i = 0, 1, 2
template <typename T>
class kd_tree
{
public:
    class node
    {
    public:
        aa_cube range; // (x0, x1] x (y0, y1] x (z0, z1]
        node *left = nullptr, *right = nullptr;
        T *points = nullptr;
        unsigned int split_dim, size = 0;

        node(const aa_cube &range, std::size_t split_dim)
            : range(range), split_dim(split_dim)
        {

        }

        ~node()
        {
            if (left)
            {
                delete left;
            }
            if (right)
            {
                delete right;
            }
            if (points)
            {
                delete points;
            }
        }
    };

    std::shared_ptr<node> root = nullptr;

    kd_tree()
    {

    }

    kd_tree(std::shared_ptr<node> &root)
        : root(root)
    {

    }

    template <typename TITERATOR>
    static kd_tree build(TITERATOR begin, TITERATOR end);

private:
    template <typename TITERATOR>
    static node *_build(TITERATOR begin, TITERATOR end,
                        const aa_cube &range, std::size_t split_dim);
};

template <typename T>
template <typename TITERATOR>
kd_tree<T> kd_tree<T>::build(TITERATOR begin, TITERATOR end)
{
    // find axis-aligned bounding box
    vector3df min_v(begin->get_dim(0), begin->get_dim(1), begin->get_dim(2));
    vector3df max_v = min_v;
    for (TITERATOR iter = begin; iter != end; ++iter)
    {
        T &point = *iter;
        if (point.get_dim(0) < min_v.x)
        {
            min_v.x = point.get_dim(0);
        }
        else if (point.get_dim(0) > max_v.x)
        {
            max_v.x = point.get_dim(0);
        }

        if (point.get_dim(1) < min_v.y)
        {
            min_v.y = point.get_dim(1);
        }
        else if (point.get_dim(1) > max_v.y)
        {
            max_v.y = point.get_dim(1);
        }

        if (point.get_dim(2) < min_v.z)
        {
            min_v.z = point.get_dim(2);
        }
        else if (point.get_dim(2) > max_v.z)
        {
            max_v.z = point.get_dim(2);
        }
    }

    std::shared_ptr<typename kd_tree<T>::node> root(
        _build(begin, end, aa_cube(min_v, max_v - min_v), 0));
    return kd_tree(std::move(root));
}

template <typename T>
template <typename TITERATOR>
typename kd_tree<T>::node *kd_tree<T>::_build(TITERATOR begin, TITERATOR end,
                                              const aa_cube &range, std::size_t split_dim)
{
    std::size_t size = end - begin;
    if (size < 128)
    {
        return nullptr;
    }

    // build current node
    unsigned int node_size = 0;
    for (TITERATOR iter = begin; iter != end; ++iter)
    {
        T &point = *iter;
        if (point.is_inside(range))
        {
            ++node_size;
        }
    }
    if (node_size < 128)
    {
        return nullptr;
    }

    typename kd_tree<T>::node *node =
        new typename kd_tree<T>::node(range, split_dim);
    node->size = node_size;
    node->points = new T[node_size];
    int i = 0;
    for (TITERATOR iter = begin; iter != end; ++iter)
    {
        T &point = *iter;
        if (point.is_inside(range))
        {
            node->points[i] = point;
            ++i;
        }
    }
    printf("kd node (%p) size=%lu\n", node, node_size);

    // split
    std::size_t median_index = node_size / 2;
    std::nth_element(node->points, node->points + median_index, node->points + node->size,
                     [split_dim] (const T &a, const T &b) -> bool
                     {
                         return a.get_dim(split_dim) < b.get_dim(split_dim);
                     });
    T &median = node->points[median_index];
    double split = median.get_dim(split_dim);
    printf("split=%lf\n", split);
    vector3df delta;
    std::size_t next_dim;
    if (split_dim == 0)
    {
        delta = vector3df(split - range.p.x, 0.0, 0.0);
        next_dim = 1;
    }
    else if (split_dim == 1)
    {
        delta = vector3df(0.0, split - range.p.y, 0.0);
        next_dim = 2;
    }
    else if (split_dim == 2)
    {
        delta = vector3df(0.0, 0.0, split - range.p.z);
        next_dim = 0;
    }
    else
    {
        // ???
        return nullptr;
    }

    node->left = _build(node->points, node->points + node->size,
                        aa_cube(range.p, range.size - delta), next_dim);
    node->right = _build(node->points, node->points + node->size,
                         aa_cube(range.p + delta, range.size - delta), next_dim);
    return node;
}

#endif // _KD_TREE_HPP_