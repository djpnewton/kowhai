#include "kowhai.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif


/**
 * @brief return the size for a given node type
 * @param type a node type to find the size of
 * @return the size in bytes
 */
static int kowhai_get_node_type_size(enum kowhai_node_type type)
{
    switch (type)
    {
        // meta tags only (no real size in the buffer)
        case BRANCH_START:
        case BRANCH_END:
            return 0;

        // normal types to describe a buffer
        case INT8_T:
        case UINT8_T:
            return 1;
        case INT16_T:
        case UINT16_T:
            return 2;
        case INT32_T:
        case UINT32_T:
        case FLOAT_T:
            return 4;
        default:
            #ifdef KOWHAI_DBG
            printf(KOWHAI_ERR" unknown item type: %d\n", type);
            #endif
            return -1;
    }
}

// calculate the complete size of a node including all the sub-elements and array items.
int kowhai_get_node_size(const struct kowhai_node_t *node, int *node_count)
{
    int size = 0;                                // size of the node 
    uint16_t i = 0;                                // items in the node 
    uint16_t _node_count = 0xFFFF;

    // if node_count is non null then limit the search items
    if (node_count != NULL)
        _node_count = *node_count;

    // if this is not a branch then just return the size of the node item (otherwise we need to drill baby)
    if (node->type != BRANCH_START)
    {
        size = kowhai_get_node_type_size(node[i].type) * node[i].count;
        goto done;
    }
    
    // this is a branch so look through all the elements in this branch and accumulate the sizes
    for (i = 1; i < _node_count; i++)
    {
        switch ((enum kowhai_node_type)node[i].type)
        {
            // navigate the hierarchy info
            case BRANCH_START:
            {
                int new_branch_size = 0;
                int new_branch_items = _node_count - i;    // remaining items
                new_branch_size = kowhai_get_node_size(&node[i], &new_branch_items);
                if (new_branch_size < 0)
                    return -2;        // propagate this down the stack
                size += new_branch_size;        // accumulate the branches size
                i += new_branch_items-1;        // skip the already processed branch items
                break;
            }
            case BRANCH_END:
                // accumulate the hole array
                size *= node[0].count;
                goto done;
            
            // accumulate the size of all the other node_count
            default:
                size += kowhai_get_node_type_size(node[i].type) * node[i].count;
                break;
        }
    }

    // did we get too many items to check
    if (i >= _node_count)
        return -3;

done:
    if (node_count != NULL)
        *node_count = i+1;
    return size;
    
}

///@brief seek/find a item in the tree given its path
///@param node to start searching from for the given item
///@param path_items number of items in the path (@todo should we just terminate the path instead)
///@param path the path of the item to seek
///@param offset set to number of bytes from the current branch to the item
///@return < 0 on failure
///@todo find the correct index (always 0 atm)
//int kowhai_get_node(struct kowhai_node_t* tree_descriptor, int num_symbols, union kowhai_symbol_t* symbols, int* offset, struct kowhai_node_t** target_node)
int kowhai_get_node(const struct kowhai_node_t *node, int *node_count, int path_items, const union kowhai_path_item *path, uint16_t *offset)
{
    int i;
    uint16_t _offset = 0;
    uint16_t _node_count = 0xFFFF;
    uint16_t found_item_size;
    int remaining_nodes;

    // if node_count is non null then limit the search items
    if (node_count != NULL)
        _node_count = *node_count;

    // look through all the items in the node list
    for (i = 0; i < _node_count; i++)
    {
        uint16_t skip_size;
        int skip_nodes;

        switch ((enum kowhai_node_type)node[i].type)
        {
            case BRANCH_END:
                // if we got a branch end then we didn't find it on this path
                return -1;
            case BRANCH_START:
            default:
                // if the path symbols match and the item array is large enough to contain our index this could be it ...
                if ((path->full.symbol == node[i].symbol) && (node[i].count > path->full.index))
                {
                    if (path_items == 1)
                    {
                        // the path fully match in values and length so this is the item we are looking for
                        goto done;
                    }
                    
                    if ((enum kowhai_node_type)node[i].type == BRANCH_START)
                    {
                        // this is not the item but it maybe in this branch so drill baby drill
                        uint16_t branch_offset = 0;                    // offset into this branch for the item
                        int branch_node_count = _node_count - i;    // max nodes to search from here
                        int ret = kowhai_get_node(&node[i+1], &branch_node_count, path_items - 1, &path[1], &branch_offset);
                        if (ret < -1)
                            // a error occurred so propagate it out
                            return ret;
                        if (ret == -1)
                            // branch ended without finding our item so continue to next item
                            break;
                        // we found the item in this branch so update _offset
                        _offset += branch_offset;
                        goto done;
                    }
                }
                break;
        }
        
        // this item is not a match so skip it (find out how many bytes and nodes to skip)
        skip_nodes = _node_count - i; // max nodes left
        skip_size = kowhai_get_node_size(&node[i], &skip_nodes);
        if (skip_size < 0)
            // propagate the error
            return skip_size;
        _offset += skip_size;
        i += skip_nodes - 1;
    }

done:
    
    // we are pointing at the root of this setting but now we need to move to the correct array index
    // all we need is (the size of this node / its count) * index bytes offset
    remaining_nodes = _node_count - i; // max nodes left
    found_item_size = kowhai_get_node_size(&node[i], &remaining_nodes);
    if (found_item_size < 0)
        return found_item_size;
    _offset += (found_item_size / node[i].count) * path->full.index;
    if (node_count != NULL)
        *node_count = i + 1;
    if (offset != NULL)
        *offset = _offset;
    return _offset;    // success
}

#if 0
/* TODO, this function should take a tree count for safety */
int kowhai_get_setting(struct kowhai_node_t* tree, int num_symbols, union kowhai_symbol_t* symbols, int* offset, struct kowhai_node_t** target_node)
{
    struct kowhai_node_t* node = tree;
    int finished = 0, finished_on_leaf = 0;
    int _offset = get_setting_offset(&node, num_symbols, symbols, 0, &finished, &finished_on_leaf);
    if (_offset > -1)
    {
        *offset = _offset;
        *target_node = node;
        return 1;
    }
    return 0;
}

int kowhai_read(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, void* result, int read_size)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (read_size > _get_node_size(node))
        return 0;
    memcpy(result, (char*)settings_buffer + offset, read_size);
    return 1;
}

int kowhai_write(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, void* value, int write_size)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (write_size > _get_node_size(node))
        return 0;
    memcpy((char*)settings_buffer + offset, value, write_size);
    return 1;
}

int kowhai_get_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, char* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_CHAR || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UCHAR))
    {
        *result = *((char*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_int16(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int16_t* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT16 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT16))
    {
        *result = *((int16_t*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_int32(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int32_t* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT32 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT32))
    {
        *result = *((uint32_t*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_float(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, float* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_FLOAT)
    {
        *result = *((float*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_set_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, char value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_CHAR || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UCHAR))
    {
        char* target_address = (char*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_int16(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int16_t value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT16 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT16))
    {
        int16_t* target_address = (int16_t*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_int32(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int32_t value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT32 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT32))
    {
        uint32_t* target_address = (uint32_t*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_float(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, float value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_FLOAT)
    {
        float* target_address = (float*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}
#endif

