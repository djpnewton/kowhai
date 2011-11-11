#ifndef _KOWHAI_H_
#define _KOWHAI_H_

#include <stdint.h>

#define KOWHAI_SYMBOL(name, array_index) ((array_index << 16) + name)
#define KOWHAI_RAW_DATA_TYPE(data_type) (data_type & (~DATA_TYPE_READONLY))

struct kowhai_symbol_parts_t
{
    uint16_t name;
    uint16_t array_index;
};

union kowhai_symbol_t
{
    uint32_t symbol;
    struct kowhai_symbol_parts_t parts;
};

// base tree descriptor node entry
struct kowhai_node_t
{
    uint16_t type;
    uint16_t symbol;
    uint16_t count;
    uint16_t data_type;
};

// tree descriptor node types
#define NODE_TYPE_BRANCH 0
#define NODE_TYPE_LEAF   1
#define NODE_TYPE_END    2

// leaf data types
#define DATA_TYPE_CHAR     0
#define DATA_TYPE_UCHAR    1
#define DATA_TYPE_INT16    2
#define DATA_TYPE_UINT16   3
#define DATA_TYPE_INT32    4
#define DATA_TYPE_UINT32   5
#define DATA_TYPE_FLOAT    6
#define DATA_TYPE_READONLY 0x8000

/* 
 * Return the size of a data type
 */
int kowhai_get_data_size(int data_type);

/* 
 * Get the memory offset (in the tree data) and the node of the tree descriptor specified by
 * a symbol path (ie, array of symbols).
 */
int kowhai_get_node(struct kowhai_node_t* tree_descriptor, int num_symbols, union kowhai_symbol_t* symbols, int* offset, struct kowhai_node_t** target_node);

/* 
 * Get the memory size of a branch of settings
 */
int kowhai_get_branch_size(struct kowhai_node_t* tree_descriptor, int* size);

/*
 * Read from a settings buffer starting at a symbol path
 * @param result, the buffer to read the result into
 * @param read_size, the number of bytes to read into the result
 */
int kowhai_read(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, void* result, int read_size);

/*
 * Write to a settings buffer starting at a symbol path
 * @param value, the buffer to write from
 * @param write_size, the number of bytes to write into the settings buffer
 */
int kowhai_write(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, void* value, int write_size);

/*
 * Get a single byte char setting specified by a symbol path from a settings buffer
 */
int kowhai_get_char(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, char* result);

/*
 * Get a 16 bit integer setting specified by a symbol path from a settings buffer
 */
int kowhai_get_int16(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int16_t* result);

/*
 * Get a 32 bit integer setting specified by a symbol path from a settings buffer
 */
int kowhai_get_int32(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int32_t* result);

/*
 * Get a 32 bit floating point setting specified by a symbol path from a settings buffer
 */
int kowhai_get_float(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, float* result);

/*
 * Set a single byte char setting specified by a symbol path in a settings buffer
 */
int kowhai_set_char(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, char value);

/*
 * Set a 16 bit integer setting specified by a symbol path in a settings buffer
 */
int kowhai_set_int16(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int16_t value);

/*
 * Set a 32 bit integer setting specified by a symbol path in a settings buffer
 */
int kowhai_set_int32(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int32_t value);

/*
 * Set a 32 bit floating point setting specified by a symbol path in a settings buffer
 */
int kowhai_set_float(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, float value);

#endif
