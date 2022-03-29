#include "vector.h"

#include "math.h"
#include <assert.h>

Vec3 vec3_set(double const x, double const y, double const z)
{
    return (Vec3){.x = x, .y = y, .z = z};
}

Vec3 vec3_set_one(double const d)
{
    return (Vec3){.x = d, .y = d, .z = d};
}

Vec3 vec3_scale(Vec3 const v, double const s)
{
    return (Vec3){.x = v.x * s, .y = v.y * s, .z = v.z * s};
}

double vec3_at(Vec3 v, int i)
{
    switch (i)
    {
    case 0:
        return v.x;
    case 1:
        return v.y;
    case 2:
        return v.z;
    default:
        assert(0);
    }
}

Vec3 vec3_add(Vec3 const a, Vec3 const b)
{
    return (Vec3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

Vec3 vec3_sub(Vec3 const a, Vec3 const b)
{
    return (Vec3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

Vec3 vec3_mul(Vec3 const a, Vec3 const b)
{
    return (Vec3){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

Vec3 vec3_div(Vec3 const a, Vec3 const b)
{
    return (Vec3){.x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z};
}

Vec3 vec3_max(Vec3 const a, Vec3 const b)
{
    return (Vec3){.x = MAX(a.x, b.x), .y = MAX(a.y, b.y), .z = MAX(a.z, b.z)};
}

Vec3 vec3_min(Vec3 const a, Vec3 const b)
{
    return (Vec3){.x = MIN(a.x, b.x), .y = MIN(a.y, b.y), .z = MIN(a.z, b.z)};
}