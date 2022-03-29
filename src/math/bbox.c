#include "bbox.h"

#include <float.h>

BBox3 bbox3_default()
{
    return (BBox3) { .pmin = vec3_set_one(DBL_MAX), .pmax = vec3_set_one(DBL_MIN) };
}

BBox3 bbox3_union(BBox3 const a, BBox3 const b)
{
    return (BBox3) { .pmin = vec3_min(a.pmin, b.pmin), .pmax = vec3_max(a.pmax, b.pmax) };
}

BBox3 bbox3_union_vec(BBox3 const b, Vec3 const v)
{
    return (BBox3) { .pmin = vec3_min(b.pmin, v), .pmax = vec3_max(b.pmax, v) };
}

Vec3 bbox3_diagonal(BBox3 const b)
{
    return vec3_sub(b.pmax, b.pmin);
}

Vec3 bbox3_centroid(BBox3 const b)
{
    return vec3_add(vec3_scale(b.pmin, 0.5), vec3_scale(b.pmax, 0.5));
}

double bbox3_surface_area(BBox3 const b)
{
    Vec3 const d = bbox3_diagonal(b);
    return 2.0 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Vec3 bbox3_offset(BBox3 const b, Vec3 const p)
{
    Vec3 o = vec3_sub(p, b.pmin);
    if (b.pmax.x > b.pmin.x) {
        o.x /= (b.pmax.x - b.pmin.x);
    }
    if (b.pmax.y > b.pmin.y) {
        o.y /= (b.pmax.y - b.pmin.y);
    }
    if (b.pmax.z > b.pmin.z) {
        o.z /= (b.pmax.z - b.pmin.z);
    }
    return o;
}

int bbox3_max_extent(BBox3 const b)
{
    Vec3 const d = bbox3_diagonal(b);
    if (d.x > d.y && d.x > d.z) {
        return 0;
    } else if (d.y > d.z) {
        return 1;
    }
    return 2;
}