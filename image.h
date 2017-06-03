#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstddef>
#include <vector>
#include <memory>

using std::size_t;
using std::ptrdiff_t;

class color_t
{
public:
    union
    {
        struct
        {
            std::uint8_t r, g, b, a;
        };
        std::uint32_t raw;
    };

    color_t(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255)
        : r(r), g(g), b(b), a(a)
    {

    }

    static color_t white;
    static color_t black;
};
#define DEFAULT_BIT_DEPTH (sizeof(color_t) * 8)

class image
{
public:
    const size_t width, height;
    const size_t bit_depth, row_size;
    std::vector<unsigned char> raw;

public:
    image(size_t width, size_t height, size_t bit_depth = DEFAULT_BIT_DEPTH);
    
    color_t &operator()(size_t x, size_t y)
    {
        return *reinterpret_cast<color_t *>(&raw[y * row_size + x * bit_depth / 8]);
    }

    const color_t &operator()(size_t x, size_t y) const
    {
        return *reinterpret_cast<const color_t *>(&raw[y * row_size + x * bit_depth / 8]);
    }

    void set_color(size_t x, size_t y, color_t color)
    {
        (*this)(x, y) = color;
    }
};

#endif // _IMAGE_H_