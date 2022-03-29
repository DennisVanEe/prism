#ifndef _PRISM_BVH_
#define _PRISM_BVH_

#include <math/bbox.h>

typedef struct BvhNode
{
    BBox3 bbox;
    union {
        int primitive_offset;
        int right_child_offset;
    };
    int num_primitives;
    int axis;
} BvhNode;

typedef struct Bvh
{
    BvhNode* nodes;
    int      num_nodes;

    void* primitives;
    int   num_primitives;
} Bvh;

typedef BBox3 (*BvhBoundFunc)(void const*);

Bvh  bvh_create(void const* primitives, int num_primitives, int primitive_size, int max_leaf_node,
                BvhBoundFunc bound_func);
void bvh_free(Bvh bvh);

#endif // _PRISM_BVH_