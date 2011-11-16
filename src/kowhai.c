#include "kowhai.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif

int kowhai_get_data_size(int setting_type)
{
    setting_type = KOWHAI_RAW_DATA_TYPE(setting_type);
    switch (setting_type)
    {
        case DATA_TYPE_CHAR:
        case DATA_TYPE_UCHAR:
            return 1;
        case DATA_TYPE_INT16:
        case DATA_TYPE_UINT16:
            return 2;
        case DATA_TYPE_INT32:
        case DATA_TYPE_UINT32:
        case DATA_TYPE_FLOAT:
            return 4;
        default:
#ifdef KOWHAI_DBG
            printf(KOWHAI_ERR" unknown setting type: %d\n", setting_type);
#endif
            return -1;
    }
}

int _get_branch_size(struct kowhai_node_t* tree_descriptor, int* steps)
{
    int size = 0;

    do
    {
        // increment step counter
        (*steps)++;

#ifdef KOWHAI_DBG
        printf(KOWHAI_INFO" tree_descriptor->type: %d, tree_descriptor->symbol: %d, tree_descriptor->count: %d\n", tree_descriptor->type, tree_descriptor->symbol, tree_descriptor->count);
#endif

        switch (tree_descriptor->type)
        {
            case NODE_TYPE_BRANCH:
            {
                // recurse into branch
                int _steps = 0;
                int branch_size = _get_branch_size(tree_descriptor + 1, &_steps);
                size += branch_size * tree_descriptor->count;
                *steps += _steps;
                tree_descriptor += _steps;
                break;
            }
            case NODE_TYPE_END:
                // return from branch
                return size;
            case NODE_TYPE_LEAF:
                // append leaf settings to size
                size += kowhai_get_data_size(tree_descriptor->data_type) * tree_descriptor->count;
                break;
            default:
#ifdef KOWHAI_DBG
                printf(KOWHAI_ERR" unknown node type (#2) %d\n", tree_descriptor->type);
#endif
                return -1;
        }
        
        // increment tree_descriptor node pointer
        tree_descriptor++;
    }
    while (1);
}

int _get_node_data_offset(struct kowhai_node_t** tree_descriptor, int num_symbols, union kowhai_symbol_t* symbols, int symbols_matched, int* finished, int root)
{
    int offset = 0;
    do
    {
#ifdef KOWHAI_DBG
        printf(KOWHAI_INFO" (*tree_descriptor)->type: %d, (*tree_descriptor)->symbol: %d, (*tree_descriptor)->count: %d\n", (*tree_descriptor)->type, (*tree_descriptor)->symbol, (*tree_descriptor)->count);
#endif

        // match symbols here
        if (symbols_matched < num_symbols)
        {
            if (symbols[symbols_matched].parts.name == (*tree_descriptor)->symbol)
            {
                symbols_matched++;
#ifdef KOWHAI_DBG
                printf(KOWHAI_INFO" symbol match, (*tree_descriptor)->symbol: %d, symbols_matched: %d\n", (*tree_descriptor)->symbol, symbols_matched);
#endif
            }
        }

        // return offset if we have matched the symbols
        if (symbols_matched == num_symbols)
        {
            int array_index = symbols[symbols_matched - 1].parts.array_index;
            if (array_index > 0)
            {
                struct kowhai_node_t* node = *tree_descriptor;
                if (array_index > node->count - 1)
                    return -1;
                switch (node->type)
                {
                    case NODE_TYPE_BRANCH:
                    {
                        int __steps;
                        offset += _get_branch_size(node + 1, &__steps) * array_index;
                        break;
                    }
                    case NODE_TYPE_LEAF:
                        offset += kowhai_get_data_size(node->data_type) * array_index;
                        break;
                }
            }

#ifdef KOWHAI_DBG
            printf(KOWHAI_INFO" return offset: %d\n", offset);
#endif
            *finished = 1;

            return offset;
        }
        
        switch ((*tree_descriptor)->type)
        {
            case NODE_TYPE_BRANCH:
            {
                // recurse into branch
                struct kowhai_node_t* branch = *tree_descriptor;
                int temp;
                (*tree_descriptor)++;
                temp = _get_node_data_offset(tree_descriptor, num_symbols, symbols, symbols_matched, finished, 0);
                if (temp == -1)
                {
#ifdef KOWHAI_DBG
                    printf(KOWHAI_ERR" branch parse failed, node symbol: %d, current offset: %d\n", branch->symbol, offset);
#endif
                    return -1;
                }
                if (!(*finished))
                    temp *= branch->count;
                else
                {
                    // branch array
                    uint16_t array_index = symbols[symbols_matched - 1].parts.array_index;
                    if (array_index > 0)
                    {
                        int __steps = 0;
                        if (array_index > branch->count - 1)
                        {
#ifdef KOWHAI_DBG
                            printf(KOWHAI_ERR" symbol match error, symbol array_index: %d, node count: %d\n", array_index, branch->count);
#endif
                            return -1;
                        }
                        temp += _get_branch_size(branch + 1, &__steps) * array_index;
                    }
                }
                offset += temp;

                // return offset if we have matched the symbols
                if (*finished)
                    return offset;

                // if this was the root branch then quit out
                if (root)
                    return -1;

                break;
            }
            case NODE_TYPE_END:
                // return out of branch
                return offset;
            case NODE_TYPE_LEAF:
            {
                // append leaf settings to offset
                int temp = kowhai_get_data_size((*tree_descriptor)->data_type);
                offset += temp * (*tree_descriptor)->count;
                break;
            }
            default:
#ifdef KOWHAI_DBG
                printf(KOWHAI_ERR" unknown node type %d\n", (*tree_descriptor)->type);
#endif
                return -1;
        }

        // increment tree_descriptor node pointer
        (*tree_descriptor)++;
    }
    while (1);
}

/* TODO, this function should take a tree descriptor count for safety */
int kowhai_get_node(struct kowhai_node_t* tree_descriptor, int num_symbols, union kowhai_symbol_t* symbols, int* offset, struct kowhai_node_t** target_node)
{
    struct kowhai_node_t* node = tree_descriptor;
    int finished = 0;
    int _offset = _get_node_data_offset(&node, num_symbols, symbols, 0, &finished, 1);
    if (_offset > -1)
    {
        *offset = _offset;
        *target_node = node;
        return 1;
    }
    return 0;
}

int _get_node_size(struct kowhai_node_t* node)
{
    switch (node->type)
    {
        case NODE_TYPE_BRANCH:
        {
            int steps = 0;
            return _get_branch_size(node + 1, &steps) * node->count;
        }
        case NODE_TYPE_LEAF:
        {
            int leaf_size = kowhai_get_data_size(node->data_type);
            leaf_size *= node->count;
            return leaf_size;
        }
        default:
            return -1;
    }
}

int kowhai_get_node_size(struct kowhai_node_t* tree_descriptor, int* size)
{
    int _size = _get_node_size(tree_descriptor);
    if (_size > -1)
    {
        *size = _size;
        return 1;
    }
    return 0;
}

int kowhai_read(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int read_offset, void* result, int read_size)
{
    struct kowhai_node_t* node;
    int node_offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &node_offset, &node))
        return 0;
    if (read_offset < 0)
        return 0;
    if (read_size + read_offset > _get_node_size(node))
        return 0;
    memcpy(result, (char*)tree_data + node_offset + read_offset, read_size);
    return 1;
}

int kowhai_write(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int write_offset, void* value, int write_size)
{
    struct kowhai_node_t* node;
    int node_offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &node_offset, &node))
        return 0;
    if (write_offset < 0)
        return 0;
    if (write_size + write_offset > _get_node_size(node))
        return 0;
    memcpy((char*)tree_data + node_offset + write_offset, value, write_size);
    return 1;
}

int kowhai_get_char(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, char* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        (KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_CHAR || KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_UCHAR))
    {
        *result = *((char*)((char*)tree_data + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_int16(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int16_t* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        (KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_INT16 || KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_UINT16))
    {
        *result = *((int16_t*)((char*)tree_data + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_int32(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int32_t* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        (KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_INT32 || KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_UINT32))
    {
        *result = *((uint32_t*)((char*)tree_data + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_float(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, float* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_FLOAT)
    {
        *result = *((float*)((char*)tree_data + offset));
        return 1;
    }
    return 0;
}

int kowhai_set_char(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, char value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        (KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_CHAR || KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_UCHAR))
    {
        char* target_address = (char*)((char*)tree_data + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_int16(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int16_t value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        (KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_INT16 || KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_UINT16))
    {
        int16_t* target_address = (int16_t*)((char*)tree_data + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_int32(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int32_t value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        (KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_INT32 || KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_UINT32))
    {
        uint32_t* target_address = (uint32_t*)((char*)tree_data + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_float(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, float value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_node(tree_descriptor, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF &&
        KOWHAI_RAW_DATA_TYPE(node->data_type) == DATA_TYPE_FLOAT)
    {
        float* target_address = (float*)((char*)tree_data + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}
