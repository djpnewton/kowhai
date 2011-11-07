#include "kowhai.h"

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif

int kowhai_get_setting_size(int setting_type)
{
    setting_type = setting_type & (~SETTING_TYPE_READONLY);
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

int _get_setting_offset(struct kowhai_node_t* tree, int num_symbols, uint16_t* symbols, int symbols_matched)
{
    int offset = 0;
    do
    {
#ifdef KOWHAI_DBG
        printf(KOWHAI_INFO" tree->type: %d, tree->name: %d, tree->count: %d\n", tree->type, tree->name, tree->count);
#endif

        // match symbols here
        if (symbols_matched < num_symbols)
        {
            if (symbols[symbols_matched] == tree->name)
                symbols_matched++;
        }

        // return offset if we have matched the symbols
        if (symbols_matched == num_symbols)
            return offset;
        
        switch (tree->type)
        {
            case NODE_TYPE_BRANCH:
            {
                // recurse into branch
                int temp = _get_setting_offset(tree + 1, num_symbols, symbols, symbols_matched);
                if (temp == -1)
                {
#ifdef KOWHAI_DBG
                    printf(KOWHAI_ERR" branch parse failed, node name: %d, current offset: %d\n", tree->name, offset);
#endif
                    return -1;
                }
                offset += temp * tree->count;
                break;
            }
            case NODE_TYPE_END:
                // return out of branch
                return offset;
            case NODE_TYPE_LEAF:
                // append leaf settings to offset
                if (tree->param1 == LEAF_TYPE_SETTING)
                {
                    int temp = kowhai_get_setting_size(tree->param2);
                    offset += temp * tree->count;
                }
                break;
            default:
#ifdef KOWHAI_DBG
                printf(KOWHAI_ERR" unknown node type %d\n", tree->type);
#endif
                return -1;
        }

        // increment tree node pointer
        tree++;
    }
    while (1);
}

int kowhai_get_setting_offset(struct kowhai_node_t* tree, int num_symbols, uint16_t* symbols)
{
    return _get_setting_offset(tree, num_symbols, symbols, 0);
}

int kowhai_get_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, char* result);
int kowhai_get_int16(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, int16_t* result);
int kowhai_get_int32(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, int32_t* result);
int kowhai_get_float(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, float* result);
int kowhai_set_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, char value);
int kowhai_set_int16(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, int16_t value);
int kowhai_set_int32(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, int32_t value);
int kowhai_set_float(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, uint16_t* symbols, float value);
