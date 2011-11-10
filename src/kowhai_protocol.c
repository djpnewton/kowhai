#include "kowhai_protocol.h"

#include <stdio.h>

int kowhai_protocol_parse(void* proto_packet, int packet_size, uint8_t* tree_id, uint8_t* cmd, struct kowhai_node_t** node, int* node_offset, int* payload_offset, void** payload)
{
#define TREE_ID_SIZE 1
#define CMD_SIZE 1
#define SYM_COUNT_SIZE 1
    int symbol_count;
    int header_plus_syms_size;
    union kowhai_symbol_t* symbols;
    
    *cmd = -1;
    *node = NULL;
    *node_offset = -1;
    *payload_offset = -1;
    *payload = NULL;

    // check packet is large enough for command byte and symbol count byte
    if (packet_size < TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE)
        return 0;

    // establish tree id
    *cmd = *((uint8_t*)proto_packet);

    // establish protocol command
    *cmd = *((uint8_t*)proto_packet + 1);

    // get symbol count
    symbol_count = *((uint8_t*)proto_packet + 2);
    header_plus_syms_size = TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE + sizeof(union kowhai_symbol_t) * symbol_count;

    // check packet is large enough for command byte, symbol count byte and symbols
    if (packet_size < header_plus_syms_size)
        return 0;

    // get symbol array
    symbols = (union kowhai_symbol_t*)((uint8_t*)proto_packet + TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE);
    
    // increment proto_packet pointer
    proto_packet = (void*)((uint8_t*)proto_packet + header_plus_syms_size);
    packet_size -= header_plus_syms_size;

    switch (*cmd)
    {
        case CMD_WRITE_SETTING:
        case CMD_WRITE_SETTING_SHADOW:
            //TODO
            break;
        case CMD_READ_SETTING:
        case CMD_READ_SETTING_SHADOW:
            //TODO
            break;
        case CMD_TRIGGER_ACTION:
            //TODO
            break;
        case CMD_READ_TREE:
            //TODO
            break;
        default:
            return 0;
    }
}
