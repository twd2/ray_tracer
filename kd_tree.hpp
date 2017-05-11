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
        unsigned int *points = nullptr; // just stores index
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
                delete [] points;
            }
        }
    };

    std::shared_ptr<node> root = nullptr;
    std::vector<T> points;

    kd_tree()
    {

    }

    kd_tree(std::shared_ptr<node> root, std::vector<T> &&points)
        : root(root), points(std::move(points))
    {

    }

    template <typename TITERATOR>
    static kd_tree build(TITERATOR begin, TITERATOR end);

private:
    static void _fill_node(node *n, const std::vector<T> &points, std::size_t depth);
};

template <typename T>
template <typename TITERATOR>
kd_tree<T> kd_tree<T>::build(TITERATOR begin, TITERATOR end)
{
    // find axis-aligned bounding box
    std::vector<T> points;
    std::vector<unsigned int> numbers;
    aa_cube begin_aabb = begin->get_aabb();
    vector3df min_v(begin_aabb.p);
    vector3df max_v(begin_aabb.p + begin_aabb.size);
    for (TITERATOR iter = begin; iter != end; ++iter)
    {
        const T &point = *iter;
        aa_cube aabb = point.get_aabb();
        vector3df p2 = aabb.p + aabb.size;
        numbers.push_back(points.size());
        points.push_back(point);
        for (std::size_t dim = 0; dim < 3; ++dim)
        {
            if (aabb.p.dim[dim] < min_v.dim[dim])
            {
                min_v.dim[dim] = aabb.p.dim[dim];
            }
            if (p2.dim[dim] > max_v.dim[dim])
            {
                max_v.dim[dim] = p2.dim[dim];
            }
        }
    }

    // build root
    std::shared_ptr<typename kd_tree<T>::node> root = std::make_shared<typename kd_tree<T>::node>(
        aa_cube(min_v, max_v - min_v), 0
    );
    root->size = numbers.size();
    root->points = new unsigned int[root->size];
    std::copy(numbers.begin(), numbers.end(), root->points);
    _fill_node(root.get(), points, 0);

    return kd_tree(root, std::move(points));
}

template <typename T>
void kd_tree<T>::_fill_node(typename kd_tree<T>::node *n,
                            const std::vector<T> &points,
                            std::size_t depth)
{
    // TODO: FIXME
    if (depth >= 100) // too deep
    {
        // printf("too deep %llu\n", end - begin);
        return;
    }

    if (depth > 0 && n->size < 16)
    {
        return;
    }

    // split
    std::size_t split_dim = n->split_dim;
    std::size_t median_index = n->size / 2;
    std::nth_element(n->points, n->points + median_index, n->points + n->size,
                     [split_dim, &points] (unsigned int a, unsigned int b) -> bool
                     {
                         return points[a].get_dim(split_dim) < points[b].get_dim(split_dim);
                     });
    unsigned int median = n->points[median_index];
    double split = points[median].get_dim(split_dim);
    //printf("split=%lf\n", split);
    vector3df delta(0.0, 0.0, 0.0);
    std::size_t next_dim;
    delta.dim[n->split_dim] = split - n->range.p.dim[n->split_dim];
    if (n->split_dim == 0)
    {
        next_dim = 1;
    }
    else if (n->split_dim == 1)
    {
        next_dim = 2;
    }
    else if (n->split_dim == 2)
    {
        next_dim = 0;
    }
    else
    {
        // ???
        return;
    }

    aa_cube left_cube(n->range.p, n->range.size - delta),
            right_cube(n->range.p + delta, n->range.size - delta);

    std::vector<unsigned int> left_points, right_points;
    left_points.reserve(n->size);
    right_points.reserve(n->size);

    typename kd_tree<T>::node 
        *left = new typename kd_tree<T>::node(left_cube, next_dim),
        *right = new typename kd_tree<T>::node(right_cube, next_dim);

    for (std::size_t i = 0; i < n->size; ++i)
    {
        const T &p = points[n->points[i]];
        if (p->is_dim_lt(n->split_dim, split))
        {
            left_points.push_back(n->points[i]);
        }
        if (p->is_dim_gte(n->split_dim, split))
        {
            right_points.push_back(n->points[i]);
        }
    }

    left->size = left_points.size();
    left->points = new unsigned int[left->size];
    std::copy(left_points.begin(), left_points.end(), left->points);

    right->size = right_points.size();
    right->points = new unsigned int[right->size];
    std::copy(right_points.begin(), right_points.end(), right->points);
    
    _fill_node(n->left, points, depth + 1);
    _fill_node(n->right, points, depth + 1);
}

#endif // _KD_TREE_HPP_