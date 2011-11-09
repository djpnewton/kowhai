#include "kowhai.h"

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif

int kowhai_get_setting_size(int setting_type)
{
    setting_type = KOWHAI_RAW_SETTING_TYPE(setting_type);
    switch (setting_type)
    {
        case SETTING_TYPE_CHAR:
        case SETTING_TYPE_UCHAR:
            return 1;
        case SETTING_TYPE_INT16:
        case SETTING_TYPE_UINT16:
            return 2;
        case SETTING_TYPE_INT32:
        case SETTING_TYPE_UINT32:
        case SETTING_TYPE_FLOAT:
            return 4;
        default:
#ifdef KOWHAI_DBG
            printf(KOWHAI_ERR" unknown setting type: %d\n", setting_type);
#endif
            return -1;
    }
}

int _get_branch_size(struct kowhai_node_t* tree, int* steps)
{
    int size = 0;

    do
    {
        // increment step counter
        (*steps)++;

#ifdef KOWHAI_DBG
        printf(KOWHAI_INFO" tree->type: %d, tree->symbol: %d, tree->count: %d\n", tree->type, tree->symbol, tree->count);
#endif

        switch (tree->type)
        {
            case NODE_TYPE_BRANCH:
            {
                // recurse into branch
                int _steps = 0;
                int branch_size = _get_branch_size(tree + 1, &_steps);
                size += branch_size * tree->count;
                *steps += _steps;
                tree += _steps;
                break;
            }
            case NODE_TYPE_END:
                // return from branch
                return size;
            case NODE_TYPE_LEAF:
                // append leaf settings to size
                if (tree->param1 == LEAF_TYPE_SETTING)
                    size += kowhai_get_setting_size(tree->param2) * tree->count;
                break;
            default:
#ifdef KOWHAI_DBG
                printf(KOWHAI_ERR" unknown node type (#2) %d\n", tree->type);
#endif
                return -1;
        }
        
        // increment tree node pointer
        tree++;
    }
    while (1);
}

int _get_setting_offset(struct kowhai_node_t** tree, int num_symbols, union kowhai_symbol_t* symbols, int symbols_matched, int* finished, int* finished_on_leaf)
{
    int offset = 0;
    do
    {
#ifdef KOWHAI_DBG
        printf(KOWHAI_INFO" (*tree)->type: %d, (*tree)->symbol: %d, (*tree)->count: %d\n", (*tree)->type, (*tree)->symbol, (*tree)->count);
#endif

        // match symbols here
        if (symbols_matched < num_symbols)
        {
            if (symbols[symbols_matched].parts.name == (*tree)->symbol)
            {
                symbols_matched++;
#ifdef KOWHAI_DBG
                printf(KOWHAI_INFO" symbol match, (*tree)->symbol: %d, symbols_matched: %d\n", (*tree)->symbol, symbols_matched);
#endif
            }
        }

        // return offset if we have matched the symbols
        if (symbols_matched == num_symbols)
        {
#ifdef KOWHAI_DBG
            printf(KOWHAI_INFO" return offset: %d\n", offset);
#endif
            *finished = 1;
            *finished_on_leaf = (*tree)->type == NODE_TYPE_LEAF;
            return offset;
        }
        
        switch ((*tree)->type)
        {
            case NODE_TYPE_BRANCH:
            {
                // recurse into branch
                struct kowhai_node_t* branch = *tree;
                int temp;
                (*tree)++;
                temp = _get_setting_offset(tree, num_symbols, symbols, symbols_matched, finished, finished_on_leaf);
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
                    // leaf array
                    if (*finished_on_leaf)
                    {
                        *finished_on_leaf = 0;
                        array_index = symbols[symbols_matched].parts.array_index;
                        if (array_index > 0)
                        {
                            struct kowhai_node_t* leaf = *tree;
                            if (leaf->type != NODE_TYPE_LEAF)
                                return -1;
                            if (array_index > leaf->count - 1)
                                return -1;
                            if (leaf->param1 != LEAF_TYPE_SETTING)
                                return -1;
                            temp += kowhai_get_setting_size(leaf->param2) * array_index;
                        }
                    }
                }
                offset += temp;

                // return offset if we have matched the symbols
                if (*finished)
                    return offset;

                break;
            }
            case NODE_TYPE_END:
                // return out of branch
                return offset;
            case NODE_TYPE_LEAF:
                // append leaf settings to offset
                if ((*tree)->param1 == LEAF_TYPE_SETTING)
                {
                    int temp = kowhai_get_setting_size((*tree)->param2);
                    offset += temp * (*tree)->count;
                }
                break;
            default:
#ifdef KOWHAI_DBG
                printf(KOWHAI_ERR" unknown node type %d\n", (*tree)->type);
#endif
                return -1;
        }

        // increment tree node pointer
        (*tree)++;
    }
    while (1);
}

/* TODO, this function should take a tree count for safety */
int kowhai_get_setting_offset(struct kowhai_node_t* tree, int num_symbols, union kowhai_symbol_t* symbols, struct kowhai_node_t** target_node)
{
    struct kowhai_node_t* node = tree;
    int finished = 0, finished_on_leaf = 0;
    int offset = _get_setting_offset(&node, num_symbols, symbols, 0, &finished, &finished_on_leaf);
    *target_node = node;
    return offset;
}

int kowhai_get_branch_size(struct kowhai_node_t* tree)
{
    int steps = 0;
    return _get_branch_size(tree + 1, &steps);
}

int kowhai_get_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, char* result)
{
    struct kowhai_node_t* node;
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
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
    int offset = kowhai_get_setting_offset(tree, num_symbols, symbols, &node);
    if (offset != -1 && node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_FLOAT)
    {
        float* target_address = (float*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}
