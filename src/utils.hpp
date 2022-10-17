#pragma once

#include <cmath>
#include <random>

namespace utils
{

    double clamp(double x, double min, double max)
    {
        if (x < min)
            return min;

        if (x > max)
            return max;

        return x;
    }

    double lerp(double a, double b, double ratio)
    {
        return a * (1.0 - ratio) + b * ratio;
    }

    bool equals(double a, double b)
    {
        return std::abs(a - b) < 1e-12;
    }

    double rand()
    {
        static auto rd{std::random_device()};
        static auto gen{std::mt19937(rd())};
        static auto dist{std::uniform_real_distribution<double>(0.0, 1.0)};

        return dist(gen);
    }

    double rand(double min, double max)
    {
        return min + (max - min) * rand();
    }
}
