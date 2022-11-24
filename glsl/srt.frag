// written for shader toy

// config

#define MAX_DEPTH 5
#define N_SAMPLES 5
#define MAX_FLOAT 1e5
#define MIN_FLOAT 1e-3

#define SPEED     0.2
#define SCALE_ON  true // not implemented
#define ROTATE_ON true


// constants

#define PI 3.14159265359

// random number generation based on https://www.shadertoy.com/view/Xt3cDn

uint base_hash(uvec2 p) {
    p = 1103515245U * ((p >> 1U) ^ (p.yx));
    uint h32 = 1103515245U * ((p.x) ^ (p.y>>3U));
    return h32 ^ (h32 >> 16);
}

float hash1(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=0.1, seed+=0.1)));
    return float(n) / float(0xffffffffU);
}

vec2 hash2(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=0.1, seed+=0.1)));
    uvec2 rz = uvec2(n, n*48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU)) / float(0x7fffffff);
}

vec3 hash3(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=0.1, seed+=0.1)));
    uvec3 rz = uvec3(n, n*16807U, n*48271U);
    return vec3(rz & uvec3(0x7fffffffU)) / float(0x7fffffff);
}

float global_seed = 0.0;

float get_seed(vec2 inp) {
    return float(base_hash(floatBitsToUint(inp))) / float(0xffffffffU) + iTime;
}

vec2 random_in_unit_disk(inout float seed) {
    vec2 h = hash2(seed) * vec2(1.0,6.28318530718);
    float phi = h.y;
    float r = sqrt(h.x);
	return r * vec2(sin(phi), cos(phi));
}

vec3 random_in_unit_sphere(inout float seed) {
    vec3 h = hash3(seed) * vec3(2.0, 6.28318530718, 1.0) - vec3(1,0,0);
    float phi = h.y;
    float r = pow(h.z, 1.0/3.0);
	return r * vec3(sqrt(1.0-h.x*h.x) * vec2(sin(phi), cos(phi)), h.x);
}

// rays

struct ray {
    vec3 origin, direction;
};

float schlick_approx(float cosine, float ior) {
    float r0 = (1.0-ior) / (1.0+ior);
    r0 = r0*r0;
    return r0 + (1.0-r0) * pow((1.0-cosine), 5.0);
}

bool does_refract(
    const in vec3 v, 
    const in vec3 n, 
    const in float ni_over_nt, 
    out vec3 refracted
    ) {
    float dt = dot(v, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt*dt);
    if (discriminant > 0.0) {
        refracted = ni_over_nt*(v - n*dt) - n*sqrt(discriminant);
        return true;
    } else { 
        return false;
    }
}

// materials

#define METAL      0
#define LAMBERTIAN 1
#define DIELECTRIC 2

struct material {
    int type;
    vec3 albedo;
    float v;
};

// hittable

struct hit_record {
    float t;
    vec3 p, normal;
    material mat;
};

struct hitable { // sphere, for now
    vec3 center;
    float radius;
};

bool hittable_hit( // only for spheres
    const in hitable h, 
    const in ray r, 
    const in float t_min, 
    const in float t_max, 
    inout hit_record rec
    ) {
    vec3 oc = r.origin - h.center;
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - h.radius * h.radius;
    
    float discriminant = b * b - c;
    if (discriminant < 0.0) {
        return false;
    }

	float s = sqrt(discriminant);
	float t1 = -b - s;
	float t2 = -b + s;
	
	float t = t1 < t_min ? t2 : t1;
    if (t < t_max && t > t_min) {
        rec.t = t;
        rec.p = r.origin + t*r.direction;
        rec.normal = (rec.p - h.center) / h.radius;

	    return true;
    } else {
        return false;
    }
}

// camera and scatter

bool scatter(
    const in ray r_in, 
    const in hit_record rec, 
    out vec3 attenuation, 
    out ray scattered
    ) {
    if(rec.mat.type == LAMBERTIAN) {
        vec3 scatter_dir = normalize(rec.normal + random_in_unit_sphere(global_seed));
        scattered = ray(rec.p, scatter_dir);
        attenuation = rec.mat.albedo;

        return true;
    } 
    
    if(rec.mat.type == METAL) {
        vec3 scatter_dir = reflect(r_in.direction, rec.normal);
        scattered = ray(rec.p, normalize(scatter_dir + rec.mat.v * random_in_unit_sphere(global_seed)));
        attenuation = rec.mat.albedo;

        return true;
    } 
    
    if(rec.mat.type == DIELECTRIC) {
        vec3 outward_normal, refracted;
        vec3 reflected = reflect(r_in.direction, rec.normal);
        float ni_over_nt, reflect_prob, cosine;
        
        attenuation = vec3(1);
        if (dot(r_in.direction, rec.normal) > 0.0) {
            outward_normal = -rec.normal;
            ni_over_nt = rec.mat.v;
            cosine = dot(r_in.direction, rec.normal);
            cosine = sqrt(1.0 - rec.mat.v * rec.mat.v * (1.0-cosine*cosine));
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / rec.mat.v;
            cosine = -dot(r_in.direction, rec.normal);
        }
        
        if (does_refract(r_in.direction, outward_normal, ni_over_nt, refracted)) {
	        reflect_prob = schlick_approx(cosine, rec.mat.v);
        } else {
            reflect_prob = 1.0;
        }
        
        if (hash1(global_seed) < reflect_prob) {
            scattered = ray(rec.p, reflected);
        } else {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }
    
    return false;
}

struct camera {
    vec3 origin, lower_left_corner, horizontal, vertical, u, v, w;
    float lens_radius;
};

camera new_camera(
    const in vec3 lookfrom, 
    const in vec3 lookat, 
    const in vec3 vup, 
    const in float vfov, 
    const in float aspect_ratio, 
    const in float aperture, 
    const in float focus_dist
    ) {
    camera cam;    
    cam.lens_radius = aperture / 2.0;
    float theta = vfov * PI / 180.0;
    float half_height = tan(theta / 2.0);
    float half_width = aspect_ratio * half_height;
    cam.origin = lookfrom;
    cam.w = normalize(lookfrom - lookat);
    cam.u = normalize(cross(vup, cam.w));
    cam.v = cross(cam.w, cam.u);
    cam.lower_left_corner = cam.origin  - half_width*focus_dist*cam.u - half_height*focus_dist*cam.v - focus_dist*cam.w;
    cam.horizontal = 2.0 * half_width * focus_dist * cam.u;
    cam.vertical = 2.0 * half_height * focus_dist * cam.v;

    return cam;
}
    
ray get_ray(camera c, vec2 uv) {
    vec2 rd = c.lens_radius * random_in_unit_disk(global_seed);
    vec3 offset = c.u * rd.x + c.v * rd.y;
    vec3 dir = normalize(c.lower_left_corner + uv.x*c.horizontal + uv.y*c.vertical - c.origin - offset);
    
    return ray(c.origin + offset, dir);
}

// scene

bool world_hit(const in ray r, const in float t_min, 
               const in float t_max, out hit_record rec) {
    rec.t = t_max;
    bool hit = false;

  	if (hittable_hit(hitable(vec3(0,-1000,-1),1000.),r,t_min,rec.t,rec)) {
        hit = true;
        rec.mat = material(LAMBERTIAN, vec3(.5),0.);
    }
  	if (hittable_hit(hitable(vec3( 0,1,0),1.),r,t_min,rec.t,rec)) {
        hit = true;
        rec.mat = material(DIELECTRIC, vec3(0),1.5);
    } 
    if (hittable_hit(hitable(vec3(-4,1,0),1.),r,t_min,rec.t,rec)) {
        hit = true;
        rec.mat = material(LAMBERTIAN, vec3(.4,.2,.1),0.);
    }       
	if (hittable_hit(hitable(vec3( 4,1,0),1.),r,t_min,rec.t,rec)) {
        hit = true;
        rec.mat = material(METAL, vec3(.7,.6,.5),0.);
    }       
    
    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {
            float m_seed = float(a) + float(b)/1000.;
            vec3 rand1 = hash3(m_seed);            
            vec3 center = vec3(float(a)+.9*rand1.x,.2,float(b)+.9*rand1.y); 
            float choose_mat = rand1.z;
            
            if (distance(center,vec3(4,.2,0)) > .9) {
                if (choose_mat < .8) { // diffuse
                    if (hittable_hit(hitable(center,.2),r,t_min,rec.t,rec)) {
                        hit=true, rec.mat=material(LAMBERTIAN, hash3(m_seed)* hash3(m_seed),0.);
                    }
                } else if (choose_mat < 0.95) { // metal
                    if (hittable_hit(hitable(center,.2),r,t_min,rec.t,rec)) {
                        hit=true, rec.mat=material(METAL,.5*(hash3(m_seed)+1.),.5*hash1(m_seed));
                    }
                } else { // glass
                    if (hittable_hit(hitable(center,.2),r,t_min,rec.t,rec)) {
                        hit=true, rec.mat=material(DIELECTRIC,vec3(0),1.5);
                    }
                }
            }
        }
    }
    
    return hit;
}

vec3 sky_color(ray r)
{
    vec3 unit_direction = normalize(r.direction);
    float t = 0.5 * (unit_direction.y + 1.0);

    vec3 day = vec3(0.5, 0.7, 1.0);
    vec3 night = vec3(0.0, 0.2, 0.5);

    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * mix(day, night, abs(sin(iTime)) * SPEED);
}

vec3 ray_color(in ray r, int depth) {
	hit_record rec;
    vec3 color = vec3(1.0);  
    
    for (int i=0; i<depth; i++) {
    	if (world_hit(r, MIN_FLOAT, MAX_FLOAT, rec)) {
            ray scattered;
            vec3 attenuation;
            if (scatter(r, rec, attenuation, scattered)) {
                color *= attenuation;
                r = scattered;
            } else {
                color *= vec3(0.0);
                break;
            }
	    } else {
            color *= sky_color(r);
            break;
    	}
    }

    return color;
}

void animate(out vec3 lookfrom) {
    if(ROTATE_ON) {
        float angle = iTime * SPEED;
    	mat4 rotation_mat1 = mat4(
            cos(angle), 0.0, -sin(angle), 0.0,
                   0.0, 1.0,        0.0, 0.0,
            sin(angle), 0.0, cos(angle), 0.0,
                   0.0, 0.0,        0.0, 1.0
        );
        mat4 rotation_mat2 = mat4(
             cos(angle), sin(angle), 0.0, 0.0,
            -sin(angle), cos(angle), 0.0, 0.0,
                    0.0,        0.0, 1.0, 0.0,
                    0.0,        0.0, 0.0, 1.0
        );
    
    	vec3 transformed = vec3(rotation_mat2 * rotation_mat1 * vec4(lookfrom, 1.0));
        // prevent camera from going through floor
        lookfrom = vec3(transformed.x, abs(transformed.y), transformed.z); 
    }

    // if(SCALE_ON) {
    // ...
    // }
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    global_seed = get_seed(fragCoord);

    float aspect_ratio = iResolution.x / iResolution.y;
    vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0);
    vec3 vup = vec3(0, 1, 0);
    float vfov = 20.0;
    float aperture = 0.1;
    float focus_dist = 10.0;

    animate(lookfrom);

    camera cam = new_camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_dist);

    vec3 color = vec3(0.0);
    for (int s=0; s<N_SAMPLES; s++)
    {
        vec2 uv = (fragCoord + hash2(global_seed))/iResolution.xy;

        ray r = get_ray(cam, uv);
        color += ray_color(r, MAX_DEPTH);
    }
    color /= float(N_SAMPLES);

    // gamma correction
    color = sqrt(color);

    fragColor = vec4(color, 1.0);
}
