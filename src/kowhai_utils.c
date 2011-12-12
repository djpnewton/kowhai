#include "kowhai_utils.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_UTILS_ERR "KOWHAI_UTILS ERROR:"
#define KOWHAI_UTILS_INFO "KOWHAI_UTILS INFO: "
#define KOWHAI_TABS "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
#endif

// forward decalre this to bring it in from kowhai.c as we need it
///@todo there should be a way to either use the public api or get at internal kowhia stuff more easily
int get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node, int initial_branch);

/**
 * @brief diff_l2r diff left tree against right tree
 * If a node is found in the left tree that is not in the right tree (ie symbol path and types/array size match) call on_unique 
 * If a node is found in both left and right tree, but the values of the node items do not match call on_diff
 * @note unique items on the right tree are ignored
 * @param left, diff this tree against right
 * @param right, diff this tree against left
 * @param on_unique, call this when a unique node is found in the left tree
 * @param on_diff, call this when a common node is found in both left and right trees and the values do not match
 * @param depth, how deep in the tree are we (0 root, 1 first branch, etc)
 */
static int diff_l2r(struct kowhai_tree_t *left, struct kowhai_tree_t *right, kowhai_on_diff_t on_unique, kowhai_on_diff_t on_diff, int swap_cb_param, int depth)
{
    int ret;
    uint16_t offset;
    struct kowhai_node_t *target_node;
    union kowhai_symbol_t symbol[1];
    int size;
    unsigned int i;
    unsigned int skip_nodes;
    struct kowhai_tree_t __left;
    struct kowhai_tree_t __right;

    // go through all the left nodes and look for matches in right 
    while (1)
    {
        // are we at the end of this branch
        if (left->desc->type == KOW_BRANCH_END)
        {
            #ifdef KOWHAI_DBG
            printf(KOWHAI_UTILS_INFO "(%d)%.*s pop\n", depth, depth, KOWHAI_TABS);
            #endif
            return KOW_STATUS_OK;
        }

        // does the left path exist in the right path
        skip_nodes = 0;
        symbol[0].parts.name = left->desc->symbol;
        symbol[0].parts.array_index = 0;
        ret = get_node(right->desc, 1, symbol, &offset, &target_node, 0);
        switch (ret)
        {
            case KOW_STATUS_OK:
                // found matching node in right 
                if (left->desc->type == KOW_BRANCH_START)
                {
                    // if the array counts match in left and right then diff the branch array items, otherwise the arrays are unique
                    ///@todo we could just diff all the items in the array if we want to allow array sizes to change 
                    if (left->desc->count == target_node->count)
                    {
                        // diff each branch array item one by one
                        __left = *left;
                        ///@todo if allowing changing array sizes then, for (i = 0; i < min(left count, right count)...
                        for (i = 0; i < left->desc->count; i++)
                        {
                            // get the offset into right for the branch array item to update
                            symbol[0].parts.array_index = i;
                            ret = get_node(right->desc, 1, symbol, &offset, NULL, 0);
                            if (ret != KOW_STATUS_OK)
                                return ret;
                            __right.desc = &target_node[1];
                            __right.data = ((uint8_t *)right->data + offset);
                            __left.desc = left->desc + 1;
                            // diff this branch array item (drill)
                            #ifdef KOWHAI_DBG
                            printf(KOWHAI_UTILS_INFO "(%d)%.*s drill\n", depth, depth, KOWHAI_TABS, left->desc->symbol);
                            #endif
                            ret = diff_l2r(&__left, &__right, on_unique, on_diff, swap_cb_param, depth + 1);
                            if (ret != KOW_STATUS_OK)
                                return ret;
                        }
                        // find the number of nodes to skip (don't skip them now as we need to point at the branch_start to find 
                        // its size below) we will skip it after that
                        skip_nodes = ((unsigned int)__left.desc - (unsigned int)left->desc) / sizeof(struct kowhai_node_t);
                        break;
                    }
                    // branch array size does not match so left is unique
                    #ifdef KOWHAI_DBG
                    printf(KOWHAI_UTILS_INFO "bpa\n");
                    #endif
                    goto unique;
                }
                else
                {
                    // if the array counts match in left and right and the types match then diff the arrays, otherwise the arrays are unique
                    ///@todo we could just diff all the items in the array if we want to allow array sizes to change 
                    ///@todo we may want to allow up casting eg from uint16_t to uint32_t
                    if ((left->desc->count == target_node->count) && (left->desc->type == target_node->type))
                    {
                        // since this is not a branch so we can just copy the whole array over
                        int ret = kowhai_get_node_size(left->desc, &size);
                        if (ret != KOW_STATUS_OK)
                            // propagate error 
                            return ret;
                        ///@todo if allowing changing array sizes then, if right size is less than left then limit to right size
                        ///@todo perhaps this should use kohai_read/write here for safety ??
                        if ((memcmp(left->data, ((uint8_t *)right->data + offset), size) != 0) && (on_diff != NULL))
                        {
                            // nodes differ from left to right so run on_diff event
                            __right.desc = target_node;
                            __right.data = (uint8_t *)right->data + offset;
                            if (!swap_cb_param)
                                ret = on_diff(left, &__right, depth);
                            else
                                ret = on_diff(&__right, left, depth);
                            if (ret != KOW_STATUS_OK)
                                return ret;
                        }
                        break;
                    }
                    // array sizes or types don't match so both are unique
                    goto unique;
                }
                break;
            case KOW_STATUS_INVALID_SYMBOL_PATH:
unique:
                // could not find matching node in right so this node is unique to the left tree
                if (on_unique != NULL)
                {
                    if (!swap_cb_param)
                        ret = on_unique(left, NULL, depth);
                    else
                        ret = on_unique(NULL, left, depth);
                    if (ret != KOW_STATUS_OK)
                        // propagate error down
                        return ret;
                }
                break;
            default:
                // propagate error down
                return ret;
        }

        // done with this node increment our position in the left tree
        // note if this was a branch skip_nodes will != 0 and contain the number of nodes and
        // kowhai_get_node_size will give the size of the whole branch
        ret = kowhai_get_node_size(left->desc, &size);
        if (ret != KOW_STATUS_OK)
            return ret;
        left->data = (uint8_t *)left->data + size;
        left->desc += skip_nodes + 1;    
        
        // if this tree is not nicely formed (ie wrapped in a branch start/end) then the next item may not be a 
        // branch end, instead we might just run off the end of the buffer so force a stop
        if (depth == 0)
            return KOW_STATUS_OK;

    }
}

/**
 * @brief diff left and right tree
 * If a node is found in the left tree that is not in the right tree (ie symbol path and types/array size match) or visa versa, call on_diff
 * If a node is found in both left and right tree, but the values of the node items do not match call on_diff
 * @param left, diff this tree against right
 * @param right, diff this tree against left
 * @param on_diff, call this when a unique node or common nodes that have different values are found
 */
int kowhai_diff(struct kowhai_tree_t *left, struct kowhai_tree_t *right, kowhai_on_diff_t on_diff)
{
    struct kowhai_tree_t _left, _right;
    int ret;

    // we use diff_l2r to find nodes that are unique in the left tree, or nodes that differ in value between left and right first
    #ifdef KOWHAI_DBG
    printf(KOWHAI_UTILS_INFO "diff left against right\n");
    #endif
    _left = *left;
    _right = *right;
    ret = diff_l2r(&_left, &_right, on_diff, on_diff, 0, 0);
    if (ret != KOW_STATUS_OK)
        return ret;

    // we just have to find nodes that are unique in the right tree. to do this we reuse diff_l2r with left and right swapped
    // and ask diff_l2r to reverse the params to the callbacks
    #ifdef KOWHAI_DBG
    printf(KOWHAI_UTILS_INFO "diff right against left\n");
    #endif
    _left = *left;
    _right = *right;
    ret = diff_l2r(&_right, &_left, on_diff, NULL, 1, 0);

    return ret;
}

/**
 * @brief called by diff when merging 
 * @param dst this is the destination node to merge common source nodes into, or NULL if node is unique to src
 * @param src this is the source node to merge into common destination nodes, or NULL if node is unique to dst
 * @param depth, how deep in the tree are we (0 root, 1 first branch, etc)
 */
static int on_diff_merge(const struct kowhai_tree_t *dst, const struct kowhai_tree_t *src, int depth)
{
    int ret;
    int size;
    uint8_t *dst_data;

    if (dst == NULL)
    {
        #ifdef KOWHAI_DBG
        printf(KOWHAI_UTILS_INFO "(%d)%.*s unique node %d in src\n", depth, depth, KOWHAI_TABS, src->desc->symbol);
        #endif
        return KOW_STATUS_OK;
    }

    if (src == NULL)
    {
        #ifdef KOWHAI_DBG
        printf(KOWHAI_UTILS_INFO "(%d)%.*s unique node %d in dst\n", depth, depth, KOWHAI_TABS, dst->desc->symbol);
        #endif
        return KOW_STATUS_OK;
    }

    // both nodes exist but differ so copy src into dst
    ret = kowhai_get_node_size(dst->desc, &size);
    if (ret != KOW_STATUS_OK)
        // propagate error 
        return ret;
    #ifdef KOWHAI_DBG
    printf(KOWHAI_UTILS_INFO "(%d)%.*s merging %d from src into dst\n", depth, depth, KOWHAI_TABS, dst->desc->symbol);
    #endif
    dst_data = (uint8_t *)dst->data;
    memcpy(dst_data, src->data, size);
    
    return KOW_STATUS_OK;
}

// merge nodes that are common to src and dst from src into dst leaving unique nodes unchanged
int kowhai_merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src)
{
    struct kowhai_tree_t *_dst = dst;

    #ifdef KOWHAI_DBG
    printf("\n");
    #endif
    // we only support merging of well formed tree's (both src and dst)
    if (dst->desc->type != KOW_BRANCH_START ||
        src->desc->type != KOW_BRANCH_START)
        return KOW_STATUS_INVALID_DESCRIPTOR;
    
    // update all the notes in dst that are common to dst and src
    return kowhai_diff(_dst, src, on_diff_merge);
}

