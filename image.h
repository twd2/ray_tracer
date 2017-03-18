#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstddef>
#include <vector>
#include <memory>

using std::size_t;
using std::ptrdiff_t;

union color_t
{
public:
    struct
    {
        std::uint8_t r, g, b, a;
    };
    std::uint32_t raw;

    color_t(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255)
        : r(r), g(g), b(b), a(a)
    {

    }

    static color_t white;
    static color_t black;
};
#define DEFAULT_BIT_DEPTH (sizeof(color_t) * 8)

static inline color_t mix_color(color_t foreground, color_t background)
{
    if (!foreground.a)
    {
        return background;
    }
    if (foreground.a == 255)
    {
        return foreground;
    }
    const std::uint32_t a = foreground.a;
    return color_t((uint32_t)foreground.r * a / 255 + (uint32_t)background.r * (255 - a) / 255,
                   (uint32_t)foreground.g * a / 255 + (uint32_t)background.g * (255 - a) / 255,
                   (uint32_t)foreground.b * a / 255 + (uint32_t)background.b * (255 - a) / 255,
                   255 /* TODO */ );
}

class image
{
public:
    std::shared_ptr<std::vector<unsigned char> > raw;
    const size_t width, height;
    const size_t bit_depth, row_size;

private:
    
public:
    image(size_t width, size_t height, size_t bit_depth = DEFAULT_BIT_DEPTH);
    
    unsigned char &operator()(size_t x, size_t y, size_t rgba_sel)
    {
        return (*raw)[y * row_size + x * bit_depth / 8 + rgba_sel];
    }
    
    const unsigned char &operator()(size_t x, size_t y, size_t rgba_sel) const
    {
        return (*raw)[y * row_size + x * bit_depth / 8 + rgba_sel];
    }

    color_t &operator()(size_t x, size_t y)
    {
        return *reinterpret_cast<color_t *>(&(*raw)[y * row_size + x * bit_depth / 8]);
    }

    const color_t &operator()(size_t x, size_t y) const
    {
        return *reinterpret_cast<const color_t *>(&(*raw)[y * row_size + x * bit_depth / 8]);
    }

    uint32_t &as_int(size_t x, size_t y)
    {
        return *reinterpret_cast<uint32_t *>(&(*raw)[y * row_size + x * bit_depth / 8]);
    }

    void set_color(size_t x, size_t y, color_t color)
    {
        if (bit_depth == DEFAULT_BIT_DEPTH)
        {
            (*this)(x, y) = color;
        }
        else if (bit_depth == 8)
        {
            (*this)(x, y, 0) = color.r;
        }
    }

    unsigned char &operator[](size_t i)
    {
        return (*raw)[i];
    }
    
    const unsigned char &operator[](size_t i) const
    {
        return (*raw)[i];
    }
};

#endif // _IMAGE_H_