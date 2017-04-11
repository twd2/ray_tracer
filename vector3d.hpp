#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <cmath>
#include <cstddef>

#ifndef M_PI
#define M_PI 3.141592653587979
#endif

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

    bool operator!=(const vector3d &v2) const
    {
        return !(*this == v2);
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
                        z * v2.x - v2.z * x,
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
        return (*this) - n * (2.0 * n.dot(*this));
    }

    vector3d refract(const vector3d &n, double n_i, double n_r) const
    {
        T i_dot_n = this->dot(n);
        T cosi2 = i_dot_n * i_dot_n / (this->length2() * n.length2());
        double n_i_n_r = n_i / n_r;
        T cosr2 = 1.0 - n_i_n_r * n_i_n_r * (1 - cosi2);
        if (cosr2 < eps)
        {
            return zero;
        }
        T cosr = sqrt(cosr2);
        if (i_dot_n >= eps)
        {
            cosr = -cosr;
        }
        return ((*this) * n_i_n_r - n * (n_i_n_r * i_dot_n + cosr)).normalize();
    }

    vector3d refract(const vector3d &n, double n_i, double n_r, double &out_cosi, double &out_cosr) const
    {
        T i_dot_n = this->dot(n);
        T cosi2 = i_dot_n * i_dot_n / (this->length2() * n.length2());
        out_cosi = sqrt(cosi2);
        double n_i_n_r = n_i / n_r;
        T cosr2 = 1.0 - n_i_n_r * n_i_n_r * (1 - cosi2);
        if (cosr2 < eps)
        {
            return zero;
        }
        T cosr = out_cosr = sqrt(cosr2);
        if (i_dot_n >= eps)
        {
            cosr = -cosr;
        }
        return ((*this) * n_i_n_r - n * (n_i_n_r * i_dot_n + cosr)).normalize();
    }

    vector3d capped() const
    {
        vector3d result = *this;
        if (result.x > 1.0)
        {
            result.x = 1.0;
        }
        if (result.y > 1.0)
        {
            result.y = 1.0;
        }
        if (result.z > 1.0)
        {
            result.z = 1.0;
        }
        return result;
    }

    bool is_zero() const
    {
        return (*this) == zero; // TODO: float.
    }

    static const vector3d zero, one, right, up, back;
};

template <typename T>
const vector3d<T> vector3d<T>::zero(T(0), T(0), T(0));

template <typename T>
const vector3d<T> vector3d<T>::one(T(1), T(1), T(1));

template <typename T>
const vector3d<T> vector3d<T>::right(T(1), T(0), T(0));

template <typename T>
const vector3d<T> vector3d<T>::up(T(0), T(1), T(0));

template <typename T>
const vector3d<T> vector3d<T>::back(T(0), T(0), T(1));

typedef vector3d<std::ptrdiff_t> vector3di;
typedef vector3d<double> vector3df;

#endif // _VECTOR3D_H_