#ifndef _KOWHAI_H_
#define _KOWHAI_H_

#include <stdint.h>

#pragma pack(1)

/*
 * @brief basic types found in a tree descriptor
 * @todo should we namespace these
 */
enum kowhai_node_type
{
    // meta tags to denote structure
    BRANCH_START = 0x0000,
    BRANCH_END,

    // types to describe buffer layout
    INT8_T = 0x0070,        ///@todo make this a reasonable balance
    UINT8_T,
    INT16_T,
    UINT16_T,
    INT32_T,
    UINT32_T,
    FLOAT_T,
};

/*
 * @brief base tree descriptor node entry
 */
struct kowhai_node_t
{
    uint16_t type;          ///< what is this node
    uint16_t symbol;        ///< index to a name for this node
    uint16_t count;         ///< if this is an array number of elements in the array (otherwise 1)
    uint16_t tag;           ///< user defined tag
};

/*
 * @brief a list of these (a path) will specify a unique address in the tree
 */
union kowhai_symbol_t
{
    uint32_t symbol;            ///< symbol of this node (really this is only 16bits max)
    struct
    {
        uint16_t name;          ///< symbol of this node
        uint16_t array_index;   ///< zero based array index of this node
    } parts;
};
#define KOWHAI_SYMBOL(name, array_index) ((array_index << 16) + name)

#pragma pack()

#define STATUS_OK                       0
#define STATUS_INVALID_SYMBOL_PATH      1
#define STATUS_INVALID_DESCRIPTOR       2
#define STATUS_INVALID_OFFSET           3
#define STATUS_NODE_DATA_TOO_SMALL      4
#define STATUS_INVALID_NODE_TYPE        5
#define STATUS_PACKET_BUFFER_TOO_SMALL  6
#define STATUS_INVALID_PROTOCOL_COMMAND 7
#define STATUS_PACKET_BUFFER_TOO_BIG    8
#define STATUS_PACKET_ 9

/**
 * @brief return the size for a given node type
 * @param type a node type to find the size of
 * @return the size in bytes
 */
int kowhai_get_node_type_size(uint16_t type);

/** 
 * @brief find a item in the tree given its path
 * @param node to start searching from for the given item
 * @param num_symbols number of items in the symbols path (@todo should we just terminate the path instead)
 * @param symbols the path of the item to find
 * @param offset set to number of bytes from the current branch to the item
 * @param target_node if return is successful this is the node that matches the symbol path
 */
int kowhai_get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node);

/**
 * @brief calculate the complete size of a node including all the sub-elements and array items.
 * @param node to find the size of
 * @param size size of the node in bytes
 * @return kowhia status
 */
int kowhai_get_node_size(const struct kowhai_node_t *node, int *size);

/*
 * Read from a tree data buffer starting at a symbol path
 * @param read_offset, the offset into the node data to start reading from
 * @param result, the buffer to read the result into
 * @param read_size, the number of bytes to read into the result
 */
int kowhai_read(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int read_offset, void* result, int read_size);

/*
 * Write to a tree data buffer starting at a symbol path
 * @param write_offset, the offset into the node data to start writing at
 * @param value, the buffer to write from
 * @param write_size, the number of bytes to write into the settings buffer
 */
int kowhai_write(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int write_offset, void* value, int write_size);

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
