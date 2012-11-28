#include "kowhai_utils.h"

#include <string.h>
#include <assert.h>

#define KOWHAI_UTILS_ERR "KOWHAI_UTILS ERROR:"
#define KOWHAI_UTILS_INFO "KOWHAI_UTILS INFO: "
#define KOWHAI_TABS "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"

static int check_nodes_match(struct kowhai_node_t* left, struct kowhai_node_t* right)
{
    if (left->type != right->type)
        return 0;
    if (left->symbol != right->symbol)
        return 0;
    return 1;
}

static int compare_simple_node_contents(struct kowhai_tree_t *left, struct kowhai_tree_t *right, void* on_diff_param, kowhai_on_diff_t on_diff, int depth)
{
    int i, ret, left_size, right_size;

    left_size = kowhai_get_node_type_size(left->desc->type);
    right_size = kowhai_get_node_type_size(right->desc->type);

    assert(left_size > 0 && right_size > 0);

    for (i = 0; i < left->desc->count; i++)
    {
        int offset = left_size * i;

        if (i < right->desc->count)
        {
            // these array elements differ if the sizes dont match or the values dont match
            int run_on_diff = 0;
            if (left_size != right_size)
                run_on_diff = 1;
            else if (memcmp((uint8_t*)left->data + offset, (uint8_t*)right->data + offset, left_size) != 0)
                run_on_diff = 1;

            // if a difference is detected then run on_diff
            if (run_on_diff && on_diff != NULL)
            {
                ret = on_diff(on_diff_param, left->desc, (uint8_t*)left->data + offset, right->desc, (uint8_t*)right->data + offset, i, depth);
                if (ret != KOW_STATUS_OK)
                    return ret;
            }
        }
        else
        {
            // call on_diff for unique array items
            if (on_diff != NULL)
            {
                ret = on_diff(on_diff_param, left->desc, (uint8_t*)left->data + offset, NULL, NULL, i, depth);
                if (ret != KOW_STATUS_OK)
                    return ret;
            }
        }
    }

    return KOW_STATUS_OK;
}

static int increment_tree(struct kowhai_tree_t* tree, int in_union)
{
    int res, size, count;
    if (!in_union)
    {
        res = kowhai_get_node_size(tree->desc, &size);
        if (res != KOW_STATUS_OK)
            return res;
        tree->data = (uint8_t*)tree->data + size;
    }
    res = kowhai_get_node_count(tree->desc, &count);
    if (res != KOW_STATUS_OK)
        return res;
    tree->desc = tree->desc + count;
    return KOW_STATUS_OK;
}

/**
 * @brief diff_l2r diff left tree against right tree
 * If a node is found in the left tree that is not in the right tree (ie symbol path and types/array size match) call on_unique 
 * If a node is found in both left and right tree, but the values of the node items do not match call on_diff
 * @note unique items on the right tree are ignored
 * @param left, diff this tree against right
 * @param left_node_index, the index of the left node in the original descriptor (used in recursive calls, original caller should pass 0)
 * @param right, diff this tree against left
 * @param right_node_index, the index of the right node in the original descriptor (used in recursive calls, original caller should pass 0)
 * @param on_diff_param, application specific parameter passed through the on_diff callback
 * @param on_diff, call this when a unique node in the left tree is found... or a common node is found in both left and right trees and the values do not match
 * @param depth, how deep in the tree are we (0 root, 1 first branch, etc)
 */
static int diff_l2r(struct kowhai_tree_t *left, struct kowhai_tree_t *right, void* on_diff_param, kowhai_on_diff_t on_diff, int depth)
{
    int ret;
    int in_union = left->desc->type == KOW_BRANCH_U_START;
    struct kowhai_tree_t left_leafs, right_leafs;

    assert(left->desc->type == KOW_BRANCH_START || in_union);
    assert(left->desc->type == right->desc->type);
    assert(left->desc->symbol == right->desc->symbol);

    // init left tree (first node to compare at this level)
    left_leafs.desc = left->desc + 1;
    left_leafs.data = left->data;

    while (left_leafs.desc->type != KOW_BRANCH_END)
    {
        int found_node_match = 0;

        // init right tree (compare against left tree at this level)
        right_leafs.desc = right->desc + 1;
        right_leafs.data = right->data;

        while (right_leafs.desc->type != KOW_BRANCH_END)
        {
            if (check_nodes_match(left_leafs.desc, right_leafs.desc))
            {
                // node metadata matches, do data comparison
                int node_is_union = left_leafs.desc->type == KOW_BRANCH_U_START;
                int node_is_branch = node_is_union || left_leafs.desc->type == KOW_BRANCH_START;
                found_node_match = 1;
                if (node_is_branch)
                {
                    // compare a complex (branch) node
                    int left_size, right_size, i;
                    ret = kowhai_get_node_size(left_leafs.desc, &left_size);
                    if (ret != KOW_STATUS_OK)
                        return ret;
                    ret = kowhai_get_node_size(right_leafs.desc, &right_size);
                    if (ret != KOW_STATUS_OK)
                        return ret;
                    for (i = 0; i < left_leafs.desc->count; i++)
                    {
                        // check for data diff in right branch array items
                        if (i < right_leafs.desc->count)
                        {
                            ret = diff_l2r(&left_leafs, &right_leafs, on_diff_param, on_diff, depth + 1);
                            if (ret != KOW_STATUS_OK)
                                return ret;
                            right_leafs.data = (uint8_t*)right_leafs.data + right_size / right_leafs.desc->count;
                        }
                        else if (on_diff != NULL)
                            // missing array items from right branch
                            on_diff(on_diff_param, left_leafs.desc, left_leafs.data, NULL, NULL, i, depth);
                        left_leafs.data = (uint8_t*)left_leafs.data + left_size / left_leafs.desc->count;
                    }
                    // rewind as increment tree will increment the data pointer
                    left_leafs.data = (uint8_t*)left_leafs.data - left_size;
                    right_leafs.data = (uint8_t*)right_leafs.data - right_size;
                }
                else
                {
                    // compare a simple (non branch) node
                    ret = compare_simple_node_contents(&left_leafs, &right_leafs, on_diff_param, on_diff, depth + 1);
                    if (ret != KOW_STATUS_OK)
                        return ret;
                }
                break;
            }
            else
            {
                // node metadata does not match increment right tree
                ret = increment_tree(&right_leafs, in_union);
                if (ret != KOW_STATUS_OK)
                    return ret;
            }
        }

        if (!found_node_match && on_diff != NULL)
            // node not found in right tree, call on_diff
            on_diff(on_diff_param, left_leafs.desc, left_leafs.data, NULL, NULL, 1, depth);

        // increment left tree (next node to compare at this level)
        ret = increment_tree(&left_leafs, in_union);
        if (ret != KOW_STATUS_OK)
            return ret;
    }

    return KOW_STATUS_OK;
}

/**
 * @brief diff left against right tree
 * If a node is found in the left tree that is not in the right tree (ie symbol path and types/array size match), call on_diff
 * If a node is found in both left and right tree, but the values of the node items do not match call on_diff
 * @param left, diff this tree against right
 * @param right, diff this tree against left
 * @param on_diff_param, application specific parameter passed through the on_diff callback
 * @param on_diff, call this when a unique (to left) node or common nodes that have different values are found
 */
int kowhai_diff(struct kowhai_tree_t *left, struct kowhai_tree_t *right, void* on_diff_param, kowhai_on_diff_t on_diff)
{
    int ret;

    // we use diff_l2r to find nodes that are unique in the left tree, or nodes that differ in value between left and right first
    KOW_LOG(KOWHAI_UTILS_INFO "diff left against right\n");
    ret = diff_l2r(left, right, on_diff_param, on_diff, 0);
    if (ret != KOW_STATUS_OK)
        return ret;

    return ret;
}

/**
 * @brief called by diff when merging
 * @param param unused parameter
 * @param dst this is the destination node to merge common source nodes into, or NULL if node is unique to src
 * @param src this is the source node to merge into common destination nodes, or NULL if node is unique to dst
 * @param depth, how deep in the tree are we (0 root, 1 first branch, etc)
 */
static int on_diff_merge(void* param, const struct kowhai_node_t *dst_node, void *dst_data, const struct kowhai_node_t *src_node, void *src_data, int index, int depth)
{
    int size;

    //
    // sanity checks for merge
    //

    if (dst_node == NULL)
    {
        KOW_LOG(KOWHAI_UTILS_INFO "(%d)%.*s unique node %d in src[%d]\n", depth, depth, KOWHAI_TABS, src_node->symbol, index);
        return KOW_STATUS_OK;
    }

    if (src_node == NULL)
    {
        KOW_LOG(KOWHAI_UTILS_INFO "(%d)%.*s unique node %d in dst[%d]\n", depth, depth, KOWHAI_TABS, dst_node->symbol, index);
        return KOW_STATUS_OK;
    }

    // if types are not the same then do not merge them
    ///@todo maybe we should allow upcasting ?
    if (dst_node->type != src_node->type)
    {
        KOW_LOG(KOWHAI_UTILS_INFO "(%d)%.*s cannot merge dst type %d in to src type %d\n", depth, depth, KOWHAI_TABS, dst_node->type, src_node->type);
        return KOW_STATUS_OK;
    }

    // if array size's are not the same then do not merge
    ///@todo maybe we should merge as much as possible
    if (dst_node->count != src_node->count)
    {
        KOW_LOG(KOWHAI_UTILS_INFO "(%d)%.*s cannot merge arrays off different sizes [dst.count = %d, src.count = %d]\n", depth, depth, KOWHAI_TABS, dst_node->count, src_node->count);
        return KOW_STATUS_OK;
    }

    //
    // both nodes exist but differ so copy src into dst
    //

    size = kowhai_get_node_type_size(dst_node->type);
    if (size == -1)
        ///@todo need a better error code here !!
        return KOW_STATUS_OK;

    KOW_LOG(KOWHAI_UTILS_INFO "(%d)%.*s merging %d bytes of %d[%d] from src into dst\n", depth, depth, KOWHAI_TABS, size, dst_node->symbol, index);
    memcpy(dst_data, src_data, size);
    
    return KOW_STATUS_OK;
}

// merge nodes that are common to src and dst from src into dst leaving unique nodes unchanged
int kowhai_merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src)
{
    KOW_LOG("\n");
    // we only support merging of well formed tree's (both src and dst)
    if (dst->desc->type != KOW_BRANCH_START ||
        src->desc->type != KOW_BRANCH_START)
        return KOW_STATUS_INVALID_DESCRIPTOR;
    
    // update all the notes in dst that are common to dst and src
    return kowhai_diff(dst, src, NULL, on_diff_merge);
}

int kowhai_create_symbol_path(struct kowhai_node_t* descriptor, struct kowhai_node_t* node, union kowhai_symbol_t* target, int* target_size)
{
    int symbol_path_index = 0;
    while (descriptor <= node)
    {
        if (symbol_path_index >= *target_size)
            return KOW_STATUS_TARGET_BUFFER_TOO_SMALL;
        target[symbol_path_index].symbol = descriptor->symbol;
        switch (descriptor->type)
        {
            case KOW_BRANCH_START:
            case KOW_BRANCH_U_START:
                symbol_path_index++;
                break;
            case KOW_BRANCH_END:
                symbol_path_index--;
                break;
            default:
                break;
        }
        descriptor++;
    }
    *target_size = symbol_path_index + 1;
    return KOW_STATUS_OK;
}

int _create_symbol_path2(struct kowhai_tree_t* tree, void* target_location, union kowhai_symbol_t* target, int* target_size, int symbol_path_length, int in_union)
{
    if (*target_size < symbol_path_length)
        return KOW_STATUS_TARGET_BUFFER_TOO_SMALL;
    while (tree->data <= target_location)
    {
        int ret;
        switch (tree->desc->type)
        {
            case KOW_BRANCH_START:
            case KOW_BRANCH_U_START:
            {
                int i;
                struct kowhai_node_t* node = tree->desc;
                for (i = 0; i < node->count; i++)
                {
                    void* p_original = tree->data;
                    target[symbol_path_length - 1].symbol = KOWHAI_SYMBOL(node->symbol, i);
                    tree->desc = node + 1;
                    ret = _create_symbol_path2(tree, target_location, target, target_size, symbol_path_length + 1, node->type == KOW_BRANCH_U_START);
                    if (ret == KOW_STATUS_OK)
                        return ret;
                    if (ret != KOW_STATUS_NOT_FOUND)
                        return ret;
                    if (node->type == KOW_BRANCH_U_START)
                    {
                        int size;
                        tree->data = p_original;
                        if (kowhai_get_node_size(node, &size) == KOW_STATUS_OK)
                            tree->data = (char*)tree->data + size / node->count;
                        else
                            return KOW_STATUS_INVALID_DESCRIPTOR;
                    }
                }
                break;
            }
            case KOW_BRANCH_END:
                return KOW_STATUS_NOT_FOUND;
            default:
            {
                int i;
                ret = kowhai_get_node_type_size(tree->desc->type);
                for (i = 0; i < tree->desc->count; i++)
                {
                    target[symbol_path_length - 1].symbol = KOWHAI_SYMBOL(tree->desc->symbol, i);
                    if ((char*)tree->data + ret * (i + 1) > (char*)target_location)
                        break;
                }
                if (!in_union)
                    tree->data = (char*)tree->data + ret * tree->desc->count;
                break;
            }
        }
        tree->desc++;
    }
    *target_size = symbol_path_length;
    return KOW_STATUS_OK;
}

int kowhai_create_symbol_path2(struct kowhai_tree_t* tree, void* target_location, union kowhai_symbol_t* target, int* target_size)
{
    struct kowhai_tree_t tmp_tree = *tree;
    // first node will be a branch
    if (*target_size < 1)
        return KOW_STATUS_TARGET_BUFFER_TOO_SMALL;
    target->symbol = tmp_tree.desc->symbol;
    tmp_tree.desc++;
    return _create_symbol_path2(&tmp_tree, target_location, target, target_size, 2, tree->desc->type == KOW_BRANCH_U_START);
}

