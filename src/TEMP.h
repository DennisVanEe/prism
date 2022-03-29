#ifndef _POLAR_BVH_
#define _POLAR_BVH_

#include <geometry/geometry.h>
#include <math/bbox.h>
#include <math/ray.h>
#include <stdbool.h>

typedef BBox3 (*BvhBoundFunc)(void const* self);
typedef bool (*BvhIntersectFunc)(void const* self, Ray ray, GeometrySurface* surface);
typedef bool (*BvhIntersectTestFunc)(void const* self, Ray ray);

typedef struct BvhPrimitive
{
    void const*          self;
    BvhBoundFunc         bound;
    BvhIntersectFunc     intersect;
    BvhIntersectTestFunc intersect_test;
} BvhPrimitive;

BBox3 bvh_primitive_bound(BvhPrimitive primitive);
bool  bvh_primitive_intersect(BvhPrimitive primitive, Ray ray, GeometrySurface* surface);
bool  bvh_primitive_intersect_test(BvhPrimitive primitive, Ray ray);

typedef struct
{
    int todo;
} BvhNode;

typedef struct Bvh
{
    BvhNode* nodes;
    int      num_nodes;

    BvhPrimitive* primitives;
    int           num_primitives;
} Bvh;

// Bvh  bvh_create(BvhPrimitive const* primitives, int num_primitives, int max_leaf_node);
void bvh_destroy(Bvh bvh);

#endif // _POLAR_BVH_