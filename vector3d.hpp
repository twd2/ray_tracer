#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <cmath>
#include <cstddef>

// All direction vector should be normalized before use.
template <typename T>
class vector3d
{
public:
    T x, y, z;

    vector3d()
        : x(T(0)), y(T(0)), z(T(0))
    {

    }

    vector3d(T x, T y, T z)
        : x(x), y(y), z(z)
    {

    }

    vector3d(const vector3d &v2)
        : x(v2.x), y(v2.y), z(v2.z)
    {

    }

    template <typename U>
    vector3d(const vector3d<U> &v2)
        : x(T(v2.x)), y(T(v2.y)), z(T(v2.z))
    {

    }

    vector3d operator-() const
    {
        return vector3d(-x, -y, -z);
    }

    vector3d operator+(const vector3d &v2) const
    {
        return vector3d(x + v2.x, y + v2.y, z + v2.z);
    }

    vector3d operator-(const vector3d &v2) const
    {
        return (*this) + (-v2);
    }

    vector3d operator*(T a) const
    {
        return vector3d(x * a, y * a, z * a);
    }

    vector3d operator/(T a) const
    {
        return vector3d(x / a, y / a, z / a);
    }

    bool operator==(const vector3d &v2) const
    {
        return x == v2.x && y == v2.y && z == v2.z;
    }

    T dot(const vector3d &v2) const
    {
        return x * v2.x + y * v2.y + z * v2.z;
    }

    T length2() const
    {
        return x * x + y * y + z * z;
    }

    T length() const
    {
        return sqrt(length2());
    }

    vector3d cross(const vector3d &v2) const
    {
        return vector3d(y * v2.z - v2.y * z,
                        x * v2.z - v2.x * z,
                        x * v2.y - v2.x * y);
    }

    vector3d modulate(const vector3d &v2) const
    {
        return vector3d(x * v2.x, y * v2.y, z * v2.z);
    }

    vector3d normalize() const
    {
        return (*this) / length();
    }

    vector3d reflect(const vector3d &n) const
    {
        return (*this) - n.dot(*this) * 2 * n;
    }

    vector3d refract(const vector3d &n) const
    {
        // TODO
    }

    bool is_zero() const
    {
        return (*this) == zero; // TODO: float.
    }

    static const vector3d zero, right, up, back;
};

template <typename T>
const vector3d<T> vector3d<T>::zero(T(0), T(0), T(0));

template <typename T>
const vector3d<T> vector3d<T>::right(T(1), T(0), T(0));

template <typename T>
const vector3d<T> vector3d<T>::up(T(0), T(1), T(0));

template <typename T>
const vector3d<T> vector3d<T>::back(T(0), T(0), T(1));

typedef vector3d<std::ptrdiff_t> vector3di;
typedef vector3d<double> vector3df;

#endif // _VECTOR3D_H_