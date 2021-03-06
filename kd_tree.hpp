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
    static kd_tree build(TITERATOR begin, TITERATOR end, bool use_median);

private:
    static void _fill_node(node *n, const std::vector<T> &points, std::size_t depth,
                           bool use_median);
};

template <typename T>
template <typename TITERATOR>
kd_tree<T> kd_tree<T>::build(TITERATOR begin, TITERATOR end, bool use_median)
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
    std::shared_ptr<node> root = std::make_shared<node>(
        aa_cube(min_v, max_v - min_v), 0
    );
    root->size = numbers.size();
    root->points = new unsigned int[root->size];
    std::copy(numbers.begin(), numbers.end(), root->points);
    _fill_node(root.get(), points, 0, use_median);

    return kd_tree(root, std::move(points));
}

template <typename T>
void kd_tree<T>::_fill_node(typename kd_tree<T>::node *n,
                            const std::vector<T> &points,
                            std::size_t depth,
                            bool use_median)
{
    if (depth >= 30) // too deep
    {
        return;
    }

    if (n->size < 16)
    {
        return;
    }

    // split
    double split;
    if (use_median)
    {
        std::size_t split_dim = n->split_dim;
        std::size_t median_index = n->size / 2;
        std::nth_element(n->points, n->points + median_index, n->points + n->size,
                         [split_dim, &points] (unsigned int a, unsigned int b) -> bool
                         {
                             return points[a].get_dim(split_dim) < points[b].get_dim(split_dim);
                         });
        unsigned int median = n->points[median_index];
        split = points[median].get_dim(split_dim);
    }
    else
    {
        split = n->range.p.dim[n->split_dim] + n->range.size.dim[n->split_dim] / 2.0;
    }

    vector3df delta(0.0, 0.0, 0.0);
    delta.dim[n->split_dim] = split - n->range.p.dim[n->split_dim];
    vector3df size_proj = n->range.size;
    size_proj.dim[n->split_dim] = 0.0;
    aa_cube left_cube(n->range.p - vector3df::one * eps,
                      size_proj + delta + vector3df::one * (2.0 * eps)),
            right_cube(n->range.p + delta - vector3df::one * eps,
                       n->range.size - delta + vector3df::one * (2.0 * eps));

    std::vector<unsigned int> left_points, right_points;
    left_points.reserve(n->size);
    right_points.reserve(n->size);

    std::size_t next_dim;
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

    n->left = new typename kd_tree<T>::node(left_cube, next_dim),
    n->right = new typename kd_tree<T>::node(right_cube, next_dim);

    for (std::size_t i = 0; i < n->size; ++i)
    {
        const T &p = points[n->points[i]];
        aa_cube aabb = p.get_aabb();
        vector3df p2 = aabb.p + aabb.size;
        if (aabb.p.dim[n->split_dim] < split + eps)
        {
            left_points.push_back(n->points[i]);
        }
        if (p2.dim[n->split_dim] >= split - eps)
        {
            right_points.push_back(n->points[i]);
        }
    }

    n->left->size = left_points.size();
    n->left->points = new unsigned int[n->left->size];
    std::copy(left_points.begin(), left_points.end(), n->left->points);

    n->right->size = right_points.size();
    n->right->points = new unsigned int[n->right->size];
    std::copy(right_points.begin(), right_points.end(), n->right->points);

    if (n->left->size < n->size)
    {
        _fill_node(n->left, points, depth + 1, use_median);
    }

    if (n->right->size < n->size)
    {
        _fill_node(n->right, points, depth + 1, use_median);
    }
}

#endif // _KD_TREE_HPP_