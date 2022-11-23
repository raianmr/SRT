#include "utils.hpp"
#include "hittable.hpp"
#include "color.hpp"

struct hit_record;

// abstract
struct material
{
    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material
{
public:
    lambertian(const color &a) : albedo(a) {}

    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        // Normals are assumed to be unit vectors in this implementation
        auto scatter_direction = rec.normal + vec3::random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    color albedo;
};

class metal : public material
{
public:
    metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        vec3 reflected = vec3::reflect(vec3::unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * vec3::random_in_unit_sphere());
        attenuation = albedo;
        return (vec3::dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = vec3::unit_vector(r_in.direction());
        double cos_theta = std::fmin(vec3::dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > utils::rand())
            direction = vec3::reflect(unit_direction, rec.normal);
        else
            direction = vec3::refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
        return true;
    }

public:
    double ir; // Index of Refraction

private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
