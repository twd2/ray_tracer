#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <cmath>
#include <cstddef>

const double eps = 1e-6;
const double eps2 = eps * eps;

// All direction vectors should be normalized before use.
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

    vector3d refract(const vector3d &i, const vector3d &n, double n_i, double n_r) const
    {
        T i_dot_n = i.dot(n);
        T cosi2 = i_dot_n * i_dot_n / (i.length2() * n.length2());
        double n_i_n_r = n_i / n_r;
        T cosr = sqrt(1.0 - n_i_n_r * n_i_n_r * (1 - cosi2));
        return i * n_i_n_r - n * (n_i_n_r * i_dot_n + cosr);
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