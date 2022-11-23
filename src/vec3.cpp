#include "vec3.hpp"

#include <cmath>

vec3::vec3(double x, double y, double z) : values{x, y, z} {}
vec3::vec3(double v) : vec3{v, v, v} {}
vec3::vec3() : vec3{0.0, 0.0, 0.0} {}

double &vec3::x() { return values[0]; }
double &vec3::y() { return values[1]; }
double &vec3::z() { return values[2]; }

double vec3::x() const { return values[0]; }
double vec3::y() const { return values[1]; }
double vec3::z() const { return values[2]; }

bool vec3::operator==(const vec3 &other) const
{
    auto double_eq = [](double a, double b) {
    };

    return utils::equals(x(), other.x()) &&
           utils::equals(y(), other.y()) &&
           utils::equals(z(), other.z());
}

bool vec3::operator!=(const vec3 &other) const
{
    return !(*this == other);
}

vec3 vec3::operator-() const
{
    return vec3{-x(), -y(), -z()};
}

double vec3::operator[](size_t index) const
{
    return values[index];
}

double &vec3::operator[](size_t index)
{
    return values[index];
}

vec3 &vec3::operator+=(const vec3 &other)
{
    x() += other.x();
    y() += other.y();
    z() += other.z();

    return *this;
}

vec3 &vec3::operator*=(const double t)
{
    x() *= t;
    y() *= t;
    z() *= t;

    return *this;
}

vec3 &vec3::operator/=(const double t)
{
    x() /= t;
    y() /= t;
    z() /= t;

    return *this;
}

vec3 vec3::operator+(const vec3 &other) const
{
    return vec3{x() + other.x(), y() + other.y(), z() + other.z()};
}

vec3 vec3::operator+(double t) const
{
    return vec3{x() + t, y() + t, z() + t};
}

vec3 vec3::operator-(const vec3 &other) const
{
    return vec3{x() - other.x(), y() - other.y(), z() - other.z()};
}

vec3 vec3::operator-(double t) const
{
    return vec3{x() - t, y() - t, z() - t};
}

vec3 vec3::operator*(const vec3 &other) const
{
    return vec3{x() * other.x(), y() * other.y(), z() * other.z()};
}

vec3 vec3::operator*(double t) const
{
    return vec3{x() * t, y() * t, z() * t};
}

vec3 vec3::operator/(const vec3 &other) const
{
    return vec3{x() / other.x(), y() / other.y(), z() / other.z()};
}

vec3 vec3::operator/(double t) const
{
    return vec3{x() / t, y() / t, z() / t};
}

double vec3::length_squared() const
{
    return x() * x() + y() * y() + z() * z();
}

double vec3::length() const
{
    return std::sqrt(length_squared());
}

double vec3::dot(const vec3 &v1, const vec3 &v2)
{
    return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}

double vec3::dot(const vec3 &other) const
{
    return dot(*this, other);
}

vec3 vec3::cross(const vec3 &v1, const vec3 &v2)
{
    return vec3{
        v1.y() * v2.z() - v1.z() * v2.y(),
        v1.z() * v2.x() - v1.x() * v2.z(),
        v1.x() * v2.y() - v1.y() * v2.x()};
}

vec3 vec3::cross(const vec3 &other) const
{
    return cross(*this, other);
}

double vec3::angle(const vec3 &v1, const vec3 &v2)
{
    return std::acos(v1.dot(v2) / (v1.length() * v2.length()));
}

double vec3::angle(const vec3 &other) const
{
    return angle(*this, other);
}

vec3 vec3::unit_vector(const vec3 &v)
{
    return v / v.length();
}

vec3 vec3::refract(const vec3 &uv, const vec3 &n, double etai_over_etat)
{
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

vec3 operator+(double t, const vec3 &v)
{
    return v + t;
}

vec3 operator-(double t, const vec3 &v)
{
    return v - t;
}

vec3 operator*(double t, const vec3 &v)
{
    return v * t;
}

vec3 operator/(double t, const vec3 &v)
{
    return v / t;
}

std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.x() << " " << v.y() << " " << v.z() << '\n';
}
