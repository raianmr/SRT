#include "vec3.hpp"

using color = vec3;

void write_color(std::ostream &out, color c)
{
    out << static_cast<int>(255.999 * c.x()) << ' '
        << static_cast<int>(255.999 * c.y()) << ' '
        << static_cast<int>(255.999 * c.z()) << '\n';
}
