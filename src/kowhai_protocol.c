#include "kowhai_protocol.h"

#include <stdio.h>
#include <string.h>

#define TREE_ID_SIZE 1
#define CMD_SIZE 1
#define SYM_COUNT_SIZE 1

int kowhai_protocol_get_tree_id(void* proto_packet, int packet_size, uint8_t* tree_id)
{
    if (packet_size < TREE_ID_SIZE)
        return STATUS_PACKET_BUFFER_TOO_SMALL;

    // establish tree id
    *tree_id = *((uint8_t*)proto_packet);

    return STATUS_OK;
}

int _parse_payload(void* payload_packet, int packet_size, struct kowhai_protocol_payload_t* payload)
{
    if (packet_size < sizeof(struct kowhai_protocol_payload_spec_t))
        return STATUS_PACKET_BUFFER_TOO_SMALL;
    memcpy(&payload->spec, payload_packet, sizeof(struct kowhai_protocol_payload_spec_t));
    if (payload->spec.size > packet_size - sizeof(struct kowhai_protocol_payload_spec_t))
        return STATUS_PACKET_BUFFER_TOO_SMALL;
    payload->data = (void*)((char*)payload_packet + sizeof(struct kowhai_protocol_payload_spec_t));
    return STATUS_OK;
}

int kowhai_protocol_parse(void* proto_packet, int packet_size, struct kowhai_protocol_t* protocol)
{
    int required_size = 0, status;

    memset(protocol, 0, sizeof(struct kowhai_protocol_t));

    // establish tree id
    status = kowhai_protocol_get_tree_id(proto_packet, packet_size, &protocol->header.tree_id);
    if (status != STATUS_OK)
        return status;

    // check packet is large enough for command byte
    required_size = TREE_ID_SIZE + CMD_SIZE;
    if (packet_size < required_size)
        return STATUS_PACKET_BUFFER_TOO_SMALL;

    // establish protocol command
    protocol->header.command = *((uint8_t*)proto_packet + 1);

    // read descriptor command requires no more parameters
    if (protocol->header.command == CMD_READ_DESCRIPTOR)
        return STATUS_OK;

    switch (protocol->header.command)
    {
        case CMD_WRITE_DATA:
        case CMD_WRITE_DATA_ACK:
        case CMD_READ_DATA:
        case CMD_READ_DATA_ACK:
        case CMD_READ_DATA_ACK_END:
            // check packet is large enough for symbol count byte
            required_size += SYM_COUNT_SIZE;
            if (packet_size < required_size)
                return STATUS_PACKET_BUFFER_TOO_SMALL;

            // get symbol count
            protocol->header.symbol_count = *((uint8_t*)proto_packet + 2);
            required_size += sizeof(union kowhai_symbol_t) * protocol->header.symbol_count;

            // check packet is large enough for symbols array
            if (packet_size < required_size)
                return STATUS_PACKET_BUFFER_TOO_SMALL;

            // get symbol array
            protocol->header.symbols = (union kowhai_symbol_t*)((uint8_t*)proto_packet + TREE_ID_SIZE + CMD_SIZE + SYM_COUNT_SIZE);
    }
    
    switch (protocol->header.command)
    {
        case CMD_WRITE_DATA:
        case CMD_WRITE_DATA_ACK:
        case CMD_READ_DATA_ACK:
        case CMD_READ_DATA_ACK_END:
        case CMD_READ_DESCRIPTOR_ACK:
        case CMD_READ_DESCRIPTOR_ACK_END:
        {
            status = _parse_payload((void*)((uint8_t*)proto_packet + required_size), packet_size - required_size, &protocol->payload);
            return status;
        }
        default:
            return STATUS_INVALID_PROTOCOL_COMMAND;
    }
}

int kowhai_protocol_create(void* proto_packet, int packet_size, struct kowhai_protocol_t* protocol, int* bytes_required)
{
    char* pkt = (char*)proto_packet;

    // write tree id
    *bytes_required = TREE_ID_SIZE;
    if (packet_size < *bytes_required)
        return STATUS_PACKET_BUFFER_TOO_SMALL;
    *pkt = protocol->header.tree_id;
    pkt += TREE_ID_SIZE;

    // write protocol command
    *bytes_required += CMD_SIZE;
    if (packet_size < *bytes_required)
        return STATUS_PACKET_BUFFER_TOO_SMALL;
    *pkt = protocol->header.command;
    pkt += CMD_SIZE;

    // read descriptor command requires no more parameters
    if (protocol->header.command == CMD_READ_DESCRIPTOR)
        return STATUS_OK;

    // check protocol command
    switch (protocol->header.command)
    {
        case CMD_WRITE_DATA:
        case CMD_WRITE_DATA_ACK:
        case CMD_READ_DATA_ACK:
        case CMD_READ_DATA:
        case CMD_READ_DATA_ACK_END:
            // write symbol count
            *bytes_required += SYM_COUNT_SIZE;
            if (packet_size < *bytes_required)
                return STATUS_PACKET_BUFFER_TOO_SMALL;
            *pkt = protocol->header.symbol_count;
            pkt += SYM_COUNT_SIZE;

            // write symbols
            *bytes_required += protocol->header.symbol_count * sizeof(union kowhai_symbol_t);
            if (packet_size < *bytes_required)
                return STATUS_PACKET_BUFFER_TOO_SMALL;
            memcpy(pkt, protocol->header.symbols, protocol->header.symbol_count * sizeof(union kowhai_symbol_t));
            pkt += protocol->header.symbol_count * sizeof(union kowhai_symbol_t);;

            break;
    }

    // read data command requires no more parameters
    if (protocol->header.command == CMD_READ_DATA)
        return STATUS_OK;

    // check protocol command
    switch (protocol->header.command)
    {
        case CMD_READ_DESCRIPTOR_ACK:
        case CMD_READ_DESCRIPTOR_ACK_END:
        case CMD_WRITE_DATA:
        case CMD_WRITE_DATA_ACK:
        case CMD_READ_DATA_ACK:
        case CMD_READ_DATA_ACK_END:
            break;
        default:
            return STATUS_INVALID_PROTOCOL_COMMAND;
    }

    // write payload spec
    *bytes_required += sizeof(struct kowhai_protocol_payload_spec_t);
    if (packet_size < *bytes_required)
        return STATUS_PACKET_BUFFER_TOO_SMALL;
    memcpy(pkt, &protocol->payload.spec, sizeof(struct kowhai_protocol_payload_spec_t));
    pkt += sizeof(struct kowhai_protocol_payload_spec_t);

    // write payload
    *bytes_required += protocol->payload.spec.size;
    if (packet_size < *bytes_required)
        return STATUS_PACKET_BUFFER_TOO_SMALL;
    memcpy(pkt, protocol->payload.data, protocol->payload.spec.size);

    return STATUS_OK;
}

int kowhai_protocol_get_overhead(struct kowhai_protocol_t* protocol, int* overhead)
{
    // check protocol command
    switch (protocol->header.command)
    {
        case CMD_READ_DESCRIPTOR:
            *overhead = TREE_ID_SIZE + CMD_SIZE;
            return STATUS_OK;
        case CMD_READ_DESCRIPTOR_ACK:
        case CMD_READ_DESCRIPTOR_ACK_END:
            *overhead = TREE_ID_SIZE + CMD_SIZE + sizeof(struct kowhai_protocol_payload_spec_t);
            return STATUS_OK;
        case CMD_WRITE_DATA:
        case CMD_WRITE_DATA_ACK:
        case CMD_READ_DATA_ACK:
        case CMD_READ_DATA_ACK_END:
            *overhead = sizeof(struct kowhai_protocol_t) - sizeof(protocol->header.symbols) +
                sizeof(union kowhai_symbol_t) * protocol->header.symbol_count -
                sizeof(protocol->payload.data);
            return STATUS_OK;
        case CMD_READ_DATA:
            *overhead = sizeof(struct kowhai_protocol_header_t) - sizeof(protocol->header.symbols) +
                sizeof(union kowhai_symbol_t) * protocol->header.symbol_count;
            return STATUS_OK;
        default:
            return STATUS_INVALID_PROTOCOL_COMMAND;
    }
}
