#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

namespace utils
{
    // Usings

    using std::make_shared;
    using std::shared_ptr;
    using std::sqrt;

    // Constants

    const double infinity = std::numeric_limits<double>::infinity();
    const double pi = 3.1415926535897932385;

    // Utility Functions

    inline double degrees_to_radians(double degrees)
    {
        return degrees * pi / 180.0;
    }

    inline double clamp(double x, double min, double max)
    {
        if (x < min)
            return min;

        if (x > max)
            return max;

        return x;
    }

    inline double lerp(double a, double b, double ratio)
    {
        return a * (1.0 - ratio) + b * ratio;
    }

    inline bool equals(double a, double b)
    {
        return std::abs(a - b) < 1e-12;
    }

    inline double rand()
    {
        static auto rd{std::random_device()};
        static auto gen{std::mt19937(rd())};
        static auto dist{std::uniform_real_distribution<double>(0.0, 1.0)};

        return dist(gen);
    }

    inline double rand(double min, double max)
    {
        return min + (max - min) * rand();
    }
}
