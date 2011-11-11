#include "kowhai_protocol.h"

#include <stdio.h>
#include <string.h>

#define TREE_ID_SIZE 1
#define CMD_SIZE 1
#define SYM_COUNT_SIZE 1

int kowhai_protocol_get_tree_id(void* proto_packet, int packet_size, uint8_t* tree_id)
{
    if (packet_size < TREE_ID_SIZE)
        return 0;

    // establish tree id
    *tree_id = *((uint8_t*)proto_packet);

    return 1;
}

int _parse_payload(void* payload_packet, int packet_size, struct kowhai_protocol_payload_t* payload)
{
    if (packet_size < sizeof(struct kowhai_protocol_payload_spec_t))
        return 0;
    memcpy(&payload->spec, payload_packet, sizeof(struct kowhai_protocol_payload_spec_t));
    if (payload->spec.size > packet_size - sizeof(struct kowhai_protocol_payload_spec_t))
        return 0;
    payload->data = (void*)((char*)payload_packet + sizeof(struct kowhai_protocol_payload_spec_t));
    return 1;
}

int kowhai_protocol_parse(void* proto_packet, int packet_size, struct kowhai_node_t* tree_descriptor, struct kowhai_protocol_t* protocol)
{
    int packet_to_syms_size;

    // establish tree id
    if (!kowhai_protocol_get_tree_id(proto_packet, packet_size, &protocol->header.tree_id))
        return 0;

    // check packet is large enough for command byte
    if (packet_size < TREE_ID_SIZE + CMD_SIZE)
        return 0;

    // establish protocol command
    protocol->header.command = *((uint8_t*)proto_packet + 1);

    // read descriptor command requires no more parameters
    if (protocol->header.command == CMD_READ_DESCRIPTOR)
    {
        protocol->header.symbol_count = 0;
        protocol->header.symbols = NULL;
        protocol->payload.spec.type = -1;
        protocol->payload.spec.count = -1;
        protocol->payload.spec.offset = -1;
        protocol->payload.spec.size = -1;
        protocol->payload.data = NULL;
        protocol->node_offset = -1;
        return 1;
    }

    // check packet is large enough for symbol count byte
    if (packet_size < TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE)
        return 0;

    // get symbol count
    protocol->header.symbol_count = *((uint8_t*)proto_packet + 2);
    packet_to_syms_size = TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE + sizeof(union kowhai_symbol_t) * protocol->header.symbol_count;

    // check packet is large enough for symbols array
    if (packet_size < packet_to_syms_size)
        return 0;

    // get symbol array
    protocol->header.symbols = (union kowhai_symbol_t*)((uint8_t*)proto_packet + TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE);
    
    switch (protocol->header.command)
    {
        case CMD_WRITE_DATA:
        {
            struct kowhai_node_t* target;
            int result;
            if (!_parse_payload((void*)((uint8_t*)proto_packet + packet_to_syms_size), packet_size - packet_to_syms_size, &protocol->payload))
                return 0;
            result = kowhai_get_node(tree_descriptor, protocol->header.symbol_count, protocol->header.symbols, &protocol->node_offset, &target);
            if (result)
            {
                protocol->node = *target;
                return 1;
            }
            return 0;
        }
        case CMD_READ_DATA:
        {
            struct kowhai_node_t* target;
            int result;
            if (!_parse_payload((void*)((uint8_t*)proto_packet + packet_to_syms_size), packet_size - packet_to_syms_size, &protocol->payload))
                return 0;
            result = kowhai_get_node(tree_descriptor, protocol->header.symbol_count, protocol->header.symbols, &protocol->node_offset, &target);
            if (result)
            {
                protocol->node = *target;
                return 1;
            }
            return 0;
        }
        default:
            return 0;
    }
}
