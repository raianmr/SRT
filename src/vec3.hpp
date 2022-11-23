#pragma once

#include "utils.hpp"

#include <iostream>

class vec3
{
private:
    double values[3];

public:
    vec3(double x, double y, double z);
    vec3(double v);
    vec3();

    double &x();
    double &y();
    double &z();

    double x() const;
    double y() const;
    double z() const;

    bool operator==(const vec3 &other) const;
    bool operator!=(const vec3 &other) const;

    vec3 operator-() const;
    double operator[](size_t index) const;
    double &operator[](size_t index);

    vec3 &operator+=(const vec3 &other);

    vec3 &operator*=(const double t);

    vec3 &operator/=(const double t);

    vec3 operator+(const vec3 &other) const;

    vec3 operator+(double t) const;

    vec3 operator-(const vec3 &other) const;

    vec3 operator-(double t) const;

    vec3 operator*(const vec3 &other) const;

    vec3 operator*(double t) const;

    vec3 operator/(const vec3 &other) const;

    vec3 operator/(double t) const;

    double length_squared() const;

    double length() const;

    static double dot(const vec3 &v1, const vec3 &v2);

    double dot(const vec3 &other) const;

    static vec3 cross(const vec3 &v1, const vec3 &v2);

    vec3 cross(const vec3 &other) const;

    static double angle(const vec3 &v1, const vec3 &v2);

    double angle(const vec3 &other) const;

    static vec3 unit_vector(const vec3 &v);

    static vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat);

    inline static vec3 random()
    {
        return {utils::rand(), utils::rand(), utils::rand()};
    }

    inline static vec3 random(double min, double max)
    {
        return {utils::rand(min, max), utils::rand(min, max), utils::rand(min, max)};
    }

    inline static vec3 random_in_unit_sphere()
    {
        while (true)
        {
            if (auto p = vec3::random(-1, 1); p.length_squared() < 1)
                return p;
        }
    }

    inline static vec3 random_in_unit_disk()
    {
        while (true)
        {
            if (auto p = vec3(utils::rand(-1, 1), utils::rand(-1, 1), 0); p.length_squared() < 1)
                return p;
        }
    }

    inline static vec3 random_unit_vector()
    {
        return unit_vector(random_in_unit_sphere());
    }

    inline static vec3 random_in_hemisphere(const vec3 &normal)
    {
        vec3 in_unit_sphere = random_in_unit_sphere();
        if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
            return in_unit_sphere;
        else
            return -in_unit_sphere;
    }

    inline bool near_zero() const
    {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (std::fabs(x()) < s) && (std::fabs(y()) < s) && (std::fabs(z()) < s);
    }

    inline static vec3 reflect(const vec3 &v, const vec3 &n)
    {
        return v - n * 2 * dot(v, n);
    }
};

vec3 operator+(double t, const vec3 &v);

vec3 operator-(double t, const vec3 &v);

vec3 operator*(double t, const vec3 &v);

vec3 operator/(double t, const vec3 &v);

std::ostream &operator<<(std::ostream &out, const vec3 &v);

using point3 = vec3;
