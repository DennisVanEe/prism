#ifndef _POLAR_MATH_BBOX_
#define _POLAR_MATH_BBOX_

#include <math/vector.h>

typedef struct
{
    Vec3 pmin;
    Vec3 pmax;
} BBox3;

BBox3 bbox3_default();

BBox3 bbox3_union(BBox3 a, BBox3 b);
BBox3 bbox3_union_vec(BBox3 b, Vec3 v);

Vec3   bbox3_diagonal(BBox3 b);
Vec3   bbox3_centroid(BBox3 b);
double bbox3_surface_area(BBox3 b);

Vec3 bbox3_offset(BBox3 b, Vec3 p);

int bbox3_max_extent(BBox3 b);

#endif // _POLAR_MATH_BBOX_