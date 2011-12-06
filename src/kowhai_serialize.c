#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define snprintf _snprintf
#endif

#include "kowhai_serialize.h"

#include <stddef.h>
#include <stdio.h>

#include <stdarg.h>
//#define snprintf debug_printf
int debug_printf(char* buf, size_t buf_size, char* format, ...)
{
    va_list args;
    va_start(args, format);
    return printf(format, args);
    va_end(args);
}

int add_header(char** dest, size_t* dest_size, int* current_offset, struct kowhai_node_t* node, kowhai_get_symbol_name_t get_name)
{
    int chars = snprintf(*dest, *dest_size,
            "{\"name\": \"%s\", \"type\": %d, \"symbol\": %d, \"count\": %d, \"tag\": %d",
            get_name(node->symbol), node->type, node->symbol, node->count, node->tag);
    if (chars >= 0)
    {
        *dest += chars;
        *dest_size -= chars;
        *current_offset += chars;
    }
    return chars;
}

int add_string(char** dest, size_t* dest_size, int* current_offset, char* string)
{
    int chars = snprintf(*dest, *dest_size, string);
    if (chars >= 0)
    {
        *dest += chars;
        *dest_size -= chars;
        *current_offset += chars;
    }
    return chars;
}

int add_indent(char** dest, size_t* dest_size, int* current_offset, int depth)
{
    int i, chars = 0;
    for (i = 0; i < depth; i++)
    {
        int c = add_string(dest, dest_size, current_offset, "\t");
        if (c < 0)
            return c;
        chars += c;
    }
    return chars;
}

int add_value(char** dest, size_t* dest_size, int* current_offset, uint16_t node_type, void* data)
{
    int chars;
    switch (node_type)
    {
        case KOW_INT8:
            chars = snprintf(*dest, *dest_size, "%d", *((int8_t*)data));
            break;
        case KOW_INT16:
            chars = snprintf(*dest, *dest_size, "%d", *((int16_t*)data));
            break;
        case KOW_INT32:
            chars = snprintf(*dest, *dest_size, "%d", *((int32_t*)data));
            break;
        case KOW_UINT8:
            chars = snprintf(*dest, *dest_size, "%d", *((uint8_t*)data));
            break;
        case KOW_UINT16:
            chars = snprintf(*dest, *dest_size, "%d", *((uint16_t*)data));
            break;
        case KOW_UINT32:
            chars = snprintf(*dest, *dest_size, "%d", *((uint32_t*)data));
            break;
        case KOW_FLOAT:
            chars = snprintf(*dest, *dest_size, "%f", *((float*)data));
            break;
        default:
            return -1;
    }
    if (chars >= 0)
    {
        *dest += chars;
        *dest_size -= chars;
        *current_offset += chars;
    }
    return chars;
}

int serialize_node(struct kowhai_node_t** desc, void* data, int data_size, int* data_offset, char* target_buffer, size_t target_size, int level, kowhai_get_symbol_name_t get_name)
{
    int target_offset = 0;
    struct kowhai_node_t* node;
    int i, chars;
    char* node_end_str;

    while (1)
    {
        node = *desc;

        if (node->type == KOW_BRANCH_END)
            return target_offset;

        // indent to current level using tabs
        chars = add_indent(&target_buffer, &target_size, &target_offset, level);
        if (chars < 0)
            return chars;

        //
        // write node
        //

        switch (node->type)
        {
            case KOW_BRANCH_START:
            {
                // write header
                chars = add_header(&target_buffer, &target_size, &target_offset, node, get_name);
                if (chars < 0)
                    return chars;
                if (node->count > 1)
                {
                    struct kowhai_node_t* initial_node = *desc;
                    // write array identifier
                    chars = add_string(&target_buffer, &target_size, &target_offset, ", \"array\": [\n");
                    if (chars < 0)
                    return chars;
                    for (i = 0; i < node->count; i++)
                    {
                        // set descriptor to initial node at the branch array
                        *desc = initial_node;
                        (*desc) += 1;
                        // indent to current level using tab
                        chars = add_indent(&target_buffer, &target_size, &target_offset, level + 1);
                        if (chars < 0)
                            return chars;
                        // write branch children start
                        chars = add_string(&target_buffer, &target_size, &target_offset, "[\n");
                        if (chars < 0)
                            return chars;
                        // write branch children
                        chars = serialize_node(desc, data, data_size, data_offset, target_buffer, target_size, level + 1, get_name);
                        if (chars < 0)
                            return chars;
                        target_offset += chars;
                        target_buffer += chars;
                        target_size -= chars;
                        // indent to current level using tab
                        chars = add_indent(&target_buffer, &target_size, &target_offset, level + 1);
                        if (chars < 0)
                            return chars;
                        // write branch children end
                        chars = add_string(&target_buffer, &target_size, &target_offset, "]");
                        if (chars < 0)
                            return chars;
                        if (i < node->count - 1)
                        {
                            chars = add_string(&target_buffer, &target_size, &target_offset, ",\n");
                            if (chars < 0)
                                return chars;
                        }
                        else
                        {
                            chars = add_string(&target_buffer, &target_size, &target_offset, "\n");
                            if (chars < 0)
                                return chars;
                        }
                    }
                }
                else
                {
                    // write children identifier
                    chars = add_string(&target_buffer, &target_size, &target_offset, ", \"children\": [\n");
                    if (chars < 0)
                        return chars;
                    // write branch children
                    (*desc) += 1;
                    chars = serialize_node(desc, data, data_size, data_offset, target_buffer, target_size, level + 1, get_name);
                    if (chars < 0)
                        return chars;
                    target_offset += chars;
                    target_buffer += chars;
                    target_size -= chars;
                }
                // indent to current level using tab
                chars = add_indent(&target_buffer, &target_size, &target_offset, level);
                if (chars < 0)
                    return chars;
                // write node end
                if (level == 0 || (*desc)[1].type == KOW_BRANCH_END)
                    node_end_str = "]}\n";
                else
                    node_end_str = "]},\n";
                chars = add_string(&target_buffer, &target_size, &target_offset, node_end_str);
                if (chars < 0)
                    return chars;
                break;
            }
            default:
            {
                int value_size = kowhai_get_node_type_size(node->type);
                // write header
                chars = add_header(&target_buffer, &target_size, &target_offset, node, get_name);
                if (chars < 0)
                    return chars;
                // write value identifier
                chars = add_string(&target_buffer, &target_size, &target_offset, ", \"value\": ");
                if (chars < 0)
                    return chars;
                // write value/s
                if (node->count > 1)
                {
                    chars = add_string(&target_buffer, &target_size, &target_offset, "[");
                    if (chars < 0)
                        return chars;
                    for (i = 0; i < node->count; i++)
                    {
                        chars = add_value(&target_buffer, &target_size, &target_offset, node->type, data);
                        if (chars < 0)
                            return chars;
                        data = (char*)data + value_size;
                        *data_offset += value_size;
                        if (i < node->count - 1)
                        {
                            chars = add_string(&target_buffer, &target_size, &target_offset, ", ");
                            if (chars < 0)
                                return chars;
                        }
                    }
                    chars = add_string(&target_buffer, &target_size, &target_offset, "]");
                    if (chars < 0)
                        return chars;
                }
                else
                {
                    chars = add_value(&target_buffer, &target_size, &target_offset, node->type, data);
                    if (chars < 0)
                        return chars;
                    data = (char*)data + value_size;
                    *data_offset += value_size;
                }
                // write node end
                if (level == 0 || node[1].type == KOW_BRANCH_END)
                    node_end_str = " }\n";
                else
                    node_end_str = " },\n";
                chars = add_string(&target_buffer, &target_size, &target_offset, node_end_str);
                if (chars < 0)
                    return chars;
                break;
            }
        }

        if (level == 0)
            return target_offset;

        (*desc) += 1;
    }
}

int kowhai_serialize(struct kowhai_node_t* descriptor, void* data, int data_size, char* target_buffer, int* target_size, kowhai_get_symbol_name_t get_name)
{
    int data_offset = 0;
    int chars = serialize_node(&descriptor, data, data_size, &data_offset, target_buffer, *target_size, 0, get_name);
    if (chars < 0)
        return KOW_STATUS_TARGET_BUFFER_TOO_SMALL;
    *target_size = chars;
    return KOW_STATUS_OK;
}
