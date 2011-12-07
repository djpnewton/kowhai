#ifndef _KOWHAI_SERIALIZE_H_
#define _KOWHAI_SERIALIZE_H_

#include "kowhai.h"

typedef char* (*kowhai_get_symbol_name_t)(uint16_t symbol);

/**
 * Convert a kowhai tree to a json ascii string
 *
 * @param descriptor, the tree descriptor
 * @param data, the tree data
 * @param data_size, the size of the tree data
 * @param target_buffer, the ascii string to write the json representation to
 * @param target_size, the size of the target buffer (upon success the number of characters written to target_buffer are returned to the caller via this parameter)
 * @param get_name, a pointer to a function that resolves kowhai symbol integers to strings
 * @return KOW_STATUS_OK if the function was successfull
 */
int kowhai_serialize(struct kowhai_node_t* descriptor, void* data, int data_size, char* target_buffer, int* target_size, kowhai_get_symbol_name_t get_name);

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
int kowhai_deserialize(char* buffer, void* scratch, int scratch_size, struct kowhai_node_t* descriptor, int* descriptor_size, void* data, int* data_size);

#endif
