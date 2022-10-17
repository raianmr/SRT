#pragma once

#include "vec3.hpp"

class ray
{
private:
    point3 org;
    vec3 dir;

public:
    ray() : org{vec3(0)}, dir{vec3(1)} {};
    ray(const point3 &_og, const vec3 &_dir)
        : org{_og}, dir{_dir} {}

    point3 origin() const { return org; }
    vec3 direction() const { return dir; }

    point3 at(double t) const
    {
        return org + dir * t;
    };
};
