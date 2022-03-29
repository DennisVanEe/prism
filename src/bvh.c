#include "bvh.h"

#include <memory_util.h>

#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define NUM_BINS 12

typedef struct BuildNode
{
    BBox3             bbox;
    struct BuildNode* left_child;
    struct BuildNode* right_child;
    int               split_axis;
    int               primitive_offset;
    int               num_primitives;
} BuildNode;

static void build_node_leaf(BuildNode* const node, int const primitive_offset,
                            int const num_primitives, BBox3 const bbox)
{
    node->bbox             = bbox;
    node->left_child       = NULL;
    node->right_child      = NULL;
    node->split_axis       = 0;
    node->primitive_offset = primitive_offset;
    node->num_primitives   = num_primitives;
}

static void build_node_interior(BuildNode* const node, BuildNode* const left_child,
                                BuildNode* const right_child)
{
    node->bbox             = bbox3_union(left_child->bbox, right_child->bbox);
    node->left_child       = left_child;
    node->right_child      = right_child;
    node->split_axis       = 0;
    node->primitive_offset = 0;
    node->num_primitives   = 0;
}

typedef struct PrimitiveInfo
{
    int   primitive_index;
    BBox3 bbox;
    Vec3  centroid;
} PrimitiveInfo;

// Predicate for below partition function:
static bool primitive_info_partition_predicate(PrimitiveInfo const primitive_info,
                                               BBox3 const centroid_bbox, int const axis,
                                               int const global_min_bin)
{
    int const b =
        MIN(NUM_BINS * vec3_at(bbox3_offset(centroid_bbox, primitive_info.centroid), axis),
            NUM_BINS - 1);
    return b <= global_min_bin;
}

static int primitive_info_partition(int const start, int const end,
                                    PrimitiveInfo* const primitive_infos, BBox3 const centroid_bbox,
                                    int const axis, int const global_min_bin)
{
    // We need to find the first case where the predicate is false:
    int first = start;
    for (; first < end; ++first)
    {
        if (!primitive_info_partition_predicate(primitive_infos[first], centroid_bbox, axis,
                                                global_min_bin))
        {
            break;
        }
    }

    if (first == end)
    {
        return first;
    }

    for (int i = first + 1; i < end; ++i)
    {
        if (primitive_info_partition_predicate(primitive_infos[i], centroid_bbox, axis,
                                               global_min_bin))
        {
            // Swap between first and i:
            PrimitiveInfo const temp = primitive_infos[i];
            primitive_infos[i]       = primitive_infos[first];
            primitive_infos[first]   = temp;
            ++first;
        }
    }

    return first;
}

// Attempts to split the cluster along a given axis. Returns true if a split
// should perform. If a split should occur, the mid point and split axis is set.
static bool split_primitive_infos(PrimitiveInfo* const primitive_infos, int const start,
                                  int const end, BBox3 const bbox, BBox3 const centroid_bbox,
                                  int* const result_mid, int* const result_axis)
{
    Vec3 const   bbox_diagonal     = bbox3_diagonal(bbox);
    double const bbox_surface_area = bbox3_surface_area(bbox);
    int const    num_primitives    = end - start;

    typedef struct Bin
    {
        BBox3 bbox;
        int   count;
    } Bin;

    Bin bins[3][NUM_BINS];

    // Initialize bins:
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < NUM_BINS; ++j)
        {
            bins[i][j].bbox  = bbox3_default();
            bins[i][j].count = 0;
        }
    }

    double global_min_cost = DBL_MAX;
    int    global_min_bin  = 0;
    int    global_min_axis = 0;

    for (int axis = 0; axis < 3; ++axis)
    {
        // Go through all the objects and place them into different sets of buckets:
        for (int i = start; i < end; ++i)
        {
            PrimitiveInfo const primitive_info = primitive_infos[i];

            int const b =
                MIN(NUM_BINS * vec3_at(bbox3_offset(centroid_bbox, primitive_info.centroid), axis),
                    NUM_BINS - 1);

            bins[axis][b].bbox = bbox3_union(bins[axis][b].bbox, primitive_info.bbox);
            bins[axis][b].count++;
        }

        // Calculate axis min cost and min bin:

        double min_cost = DBL_MAX;
        int    min_bin  = 0;

        for (int b = 0; b < NUM_BINS - 1; ++b)
        {
            Bin left_bins = {.bbox = bbox3_default(), .count = 0};
            for (int i = 0; i <= b; ++i)
            {
                left_bins.bbox = bbox3_union(left_bins.bbox, bins[axis][b].bbox);
                left_bins.count += bins[axis][b].count;
            }

            Bin right_bins = {.bbox = bbox3_default(), .count = 0};
            for (int i = b + 1; i < NUM_BINS; ++i)
            {
                right_bins.bbox = bbox3_union(right_bins.bbox, bins[axis][b].bbox);
                right_bins.count += bins[axis][b].count;
            }

            int const cost = 0.125 + (left_bins.count * bbox3_surface_area(left_bins.bbox) +
                                      right_bins.count * bbox3_surface_area(right_bins.bbox)) /
                                         bbox_surface_area;

            if (cost < min_cost)
            {
                min_cost = cost;
                min_bin  = b;
            }
        }

        // Now check it against the global minimum:
        if (min_cost < global_min_cost)
        {
            global_min_cost = min_cost;
            global_min_bin  = min_bin;
            global_min_axis = axis;
        }
    }

    // Now check if we should perform a split or not. If we assume that every leaf has a cost of 1,
    // then we have that it's not worth it if the cost is greater or equal to the number of objects:
    if (global_min_cost >= num_primitives)
    {
        return false;
    }

    // Now we attempt to perform a partition:
    *result_mid  = primitive_info_partition(start, end, primitive_infos, centroid_bbox,
                                            global_min_axis, global_min_bin);
    *result_axis = global_min_axis;

    return true;
}

static BuildNode* bvh_recursive_build(MemoryArena* const   arena,
                                      PrimitiveInfo* const primitive_infos, int const start,
                                      int const end, unsigned char const* const primitives,
                                      int const primitive_size, int* const total_nodes,
                                      int* const           ordered_primitive_index,
                                      unsigned char* const ordered_primitives,
                                      int const            max_leaf_node)
{
    (*total_nodes)++;
    BuildNode* const node = memory_arena_allocate(arena, sizeof(BuildNode));

    BBox3 bbox = bbox3_default();
    for (int i = start; i < end; ++i)
    {
        bbox = bbox3_union(bbox, primitive_infos[i].bbox);
    }

    int const num_primitives = end - start;

    // If it's less than 1, it's a leaf node:
    if (num_primitives == 1)
    {
        build_node_leaf(node, *ordered_primitive_index, 1, bbox);

        memcpy(&ordered_primitives[((*ordered_primitive_index)++) * primitive_size],
               &primitives[primitive_infos[start].primitive_index * primitive_size],
               primitive_size);

        return node;
    }

    BBox3 centroid_bbox = bbox3_default();
    for (int i = start; i < end; ++i)
    {
        centroid_bbox = bbox3_union_vec(centroid_bbox, primitive_infos[i].centroid);
    }

    int mid  = 0;
    int axis = 0;
    if (split_primitive_infos(primitive_infos, start, end, bbox, centroid_bbox, &mid, &axis))
    {
        BuildNode* const left_child = bvh_recursive_build(
            arena, primitive_infos, start, mid, primitives, primitive_size, total_nodes,
            ordered_primitive_index, ordered_primitives, max_leaf_node);
        BuildNode* const right_child = bvh_recursive_build(
            arena, primitive_infos, mid, end, primitives, primitive_size, total_nodes,
            ordered_primitive_index, ordered_primitives, max_leaf_node);

        build_node_interior(node, left_child, right_child);

        return node;
    }

    // Create a leaf node:
    build_node_leaf(node, *ordered_primitive_index, num_primitives, bbox);
    for (int i = start; i < end; ++i)
    {
        memcpy(&ordered_primitives[((*ordered_primitive_index)++) * primitive_size],
               &primitives[primitive_infos[i].primitive_index * primitive_size], primitive_size);
    }
    return node;
}

static int bvh_flatten(BvhNode* const nodes, BuildNode* const build_node, int* offset)
{
    BvhNode* const node = &nodes[*offset];
    node->bbox          = build_node->bbox;

    int const curr_offset = (*offset)++;
    if (build_node->num_primitives > 0)
    {
        node->primitive_offset = build_node->primitive_offset;
        node->num_primitives   = build_node->num_primitives;
        return curr_offset;
    }

    node->axis           = build_node->split_axis;
    node->num_primitives = 0;

    bvh_flatten(nodes, build_node->left_child, offset);
    node->right_child_offset = bvh_flatten(nodes, build_node->right_child, offset);

    return curr_offset;
}

Bvh bvh_create(void const* const primitives, int const num_primitives, int const primitive_size,
               int const max_leaf_node, BvhBoundFunc const bound_func)
{
    Bvh result;

    unsigned char const* const primitives_bytes = primitives;

    PrimitiveInfo* const primitive_info = check_malloc(num_primitives * sizeof(PrimitiveInfo));
    for (int i = 0; i < num_primitives; ++i)
    {
        primitive_info[i].primitive_index = i;
        primitive_info[i].bbox            = bound_func(&primitives_bytes[i * primitive_size]);
        primitive_info[i].centroid        = bbox3_centroid(primitive_info[i].bbox);
    }

    // Allocate space for the primitives:
    result.primitives     = check_malloc(num_primitives * primitive_size);
    result.num_primitives = num_primitives;

    MemoryArena arena = memory_arena_create(sizeof(BuildNode) * 256);

    int total_nodes             = 0;
    int ordered_primitive_index = 0;

    BuildNode* const root_node = bvh_recursive_build(
        &arena, primitive_info, 0, num_primitives, primitives, primitive_size, &total_nodes,
        &ordered_primitive_index, result.primitives, max_leaf_node);

    result.nodes     = check_malloc(total_nodes * sizeof(BvhNode));
    result.num_nodes = total_nodes;

    int flatten_offset = 0;
    bvh_flatten(result.nodes, root_node, &flatten_offset);

    memory_arena_destroy(arena);
}

void bvh_free(Bvh bvh)
{
    free(bvh.nodes);
    free(bvh.primitives);
}