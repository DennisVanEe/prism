#ifndef _POLAR_MATH_VECTOR_
#define _POLAR_MATH_VECTOR_

typedef struct
{
    double x;
    double y;
    double z;
} Vec3;

Vec3 vec3_set(double x, double y, double z);
Vec3 vec3_set_one(double d);

Vec3 vec3_scale(Vec3 v, double s);

double vec3_at(Vec3 v, int i);

Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 a, Vec3 b);
Vec3 vec3_div(Vec3 a, Vec3 b);

Vec3 vec3_max(Vec3 a, Vec3 b);
Vec3 vec3_min(Vec3 a, Vec3 b);

#endif // _POLAR_MATH_VECTOR_