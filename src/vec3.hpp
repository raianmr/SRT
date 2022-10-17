#pragma once

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

    static vec3 normalize(const vec3 &v);
};

using point3 = vec3;

std::ostream &operator<<(std::ostream &out, const vec3 &v);

vec3 operator+(double t, const vec3 &v);

vec3 operator-(double t, const vec3 &v);

vec3 operator*(double t, const vec3 &v);

vec3 operator/(double t, const vec3 &v);
