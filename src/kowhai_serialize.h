#ifndef _KOWHAI_SERIALIZE_H_
#define _KOWHAI_SERIALIZE_H_

#include "kowhai.h"

/**
 * @brief callback used to convert a kowhai symbol id to its string representation
 * @param param application specific parameter passed through
 * @param symbol the symbol id to convert
 * @return the string representation of the symbol id
 */
typedef char* (*kowhai_get_symbol_name_t)(void* param, uint16_t symbol);

/**
 * @brief callback used to convert a kowhai symbol string to its kowhai_symbol_t representation
 * @param param application specific parameter passed through
 * @param symbol the symbol string to convert
 * @param len  length of symbol string (it is not NULL terminated !!)
 * @return string symbol id or -1 if symbol is not found
 */
typedef int (*kowhai_get_symbol_t)(void* param, const char *symbol, int len);

/**
 * @brief callback used to convert a kowhai symbol string to its kowhai_symbol_t representation
 * @param param application specific parameter passed through
 * @return 0 to continue, -1 to return error
 */
typedef int (*kowhai_node_not_found_t)(void* param, union kowhai_symbol_t *path, int path_len);

/**
 * @brief convert a string with symbol names separated by '.' delimiters and arrays '[]' into a kowhai_symbol_t array
 * @param path_str path string to convert (symbols should be separated by '.' chars and array index designated by '[2]' for example, 0 is assumed if index not present
 * @param path_strlen number of chars in the above string
 * @param path destination populated with kowhai_symbol_t to make the path
 * @param path_len size of path (number of kowhai_symbol_t allocated), updated on KOW_STATUS_OK to the number of symbols populated
 * @param get_name_param passed to the callback below
 * @param get_name called supplied callback, given a symbol string it returns the symbol ID
 * @return standard kowhai returns (ie KOW_STATUS_OK on success, else error)
 */
int kowhai_str_to_path(const char *path_str, int path_strlen, union kowhai_symbol_t *path, int *path_len, void *get_name_param, kowhai_get_symbol_t get_name);

/**
 * Convert a kowhai tree to a json ascii string
 *
 * @param tree, the kowhai tree
 * @param target_buffer, the ascii string to write the json representation to
 * @param target_size, the size of the target buffer (upon success the number of characters written to target_buffer are returned to the caller via this parameter)
 * @param get_name_param application specific parameter passed through the get_name callback
 * @param get_name, a pointer to a function that resolves kowhai symbol integers to strings
 * @return KOW_STATUS_OK if the function was successfull
 */
int kowhai_serialize_tree(struct kowhai_tree_t tree, char* target_buffer, int* target_size, void* get_name_param, kowhai_get_symbol_name_t get_name);

/**
 * Serialize all the nodes in a tree to a jason ascii string format
 * This differs from kowhai_serialize_tree in that it cannot create a new tree when de-serialized, 
 * it can only merge the de-serialized nodes into an existing tree (see kowhai_deserialize_nodes)
 * @param dst, put the serialized jason string here
 * @param dst_len, size of dst in characters
 * @param src_tree, tree to serialize
 * @param path, working buffer to store the running path in (must be large enough to encode the whole tree)
 * @param path_len, size of above path (if too small to encode the whole tree this will fail)
 * @param get_name_param argument for above callback
 * @param get_name called to convert path value to string
 */
int kowhai_serialize_nodes(char *dst, int *dst_len, struct kowhai_tree_t *src_tree, union kowhai_symbol_t *path, int path_len, void* get_name_param, kowhai_get_symbol_name_t get_name);

/**
 * Convert a json ascii string to a kowhai tree
 *
 * @param buffer, the json string
 * @param scratch, a buffer to be used by the json parser
 * @param scratch_size, the size of the scatch buffer
 * @param descriptor, the tree descriptor to be written
 * @param descriptor_size, the tree descriptor size (returns size of tree descriptor as written on success)
 * @param data, the tree data to be written
 * @param data_size, the size of the tree data (returns size of tree data as written on success)
 * @return KOW_STATUS_OK if the function was successfull
 */
int kowhai_deserialize_tree(char* buffer, void* scratch, int scratch_size, struct kowhai_node_t* descriptor, int* descriptor_size, void* data, int* data_size);


/**
 * Deserialize all the nodes in a tree to a jason ascii string format back into a tree
 * This differs from kowhai_deserialize_tree in that it cannot create a new tree it can only 
 * merge the de-serialized nodes into an existing tree (see kowhai_serialize_nodes)
 * @param src, buffer containing nodes in jason ascii format
 * @param src_len, size of the src buffer in bytes
 * @param dst_tree, deserialize the src buffer in to this tree
 * @param path, working buffer to store the running path in (must be large enough to encode the whole tree)
 * @param path_len, size of above path (if too small to encode the whole tree this will fail)
 * @param get_name_param argument for callback below
 * @param get_name called to convert symbols from strings back to numerical values
 * @param not_found_param argument for callback below
 * @param not_found called when a node is not found in the dst_tree, if it returns 0 the error is ignored
 */
int kowhai_deserialize_nodes(char* src, int src_size, struct kowhai_tree_t *dst_tree, union kowhai_symbol_t *path, int path_len, void* scratch, int scratch_size, void *get_name_param, kowhai_get_symbol_t get_name, void *not_found_param, kowhai_node_not_found_t not_found);

#endif
