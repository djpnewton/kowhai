#ifndef _KOWHAI_SERIALIZE_H_
#define _KOWHAI_SERIALIZE_H_

#include "kowhai.h"

typedef char* (*kowhai_get_symbol_name_t)(uint16_t symbol);

int kowhai_serialize(struct kowhai_node_t* descriptor, void* data, int data_size, char* target_buffer, int target_size, kowhai_get_symbol_name_t get_name);

#endif
