#ifndef _KOWHAI_PROTOCOL_H_
#define _KOWHAI_PROTOCOL_H_

#include "kowhai.h" 

//
// Protocol commands
//

// Write tree data
#define CMD_WRITE_DATA              0x00
// Acknowledge write tree data command
#define CMD_WRITE_DATA_ACK          0x0F
// Read tree data
#define CMD_READ_DATA               0x10
// Acknowledge read tree data command (and return the data)
#define CMD_READ_DATA_ACK           0x1F
// Acknowledge read tree data command (this is the final packet)
#define CMD_READ_DATA_ACK_END       0x1E
// Read the tree descriptor
#define CMD_READ_DESCRIPTOR         0xF0
// Acknowledge read tree command (and return tree contents)
#define CMD_READ_DESCRIPTOR_ACK     0xFF
// Acknowledge read tree command (this is the final packet)
#define CMD_READ_DESCRIPTOR_ACK_END 0xFE

//
// Protocol structures
//

#pragma pack(1)

struct kowhai_protocol_header_t
{
    uint8_t tree_id;
    uint8_t command;
    uint8_t symbol_count;
    union kowhai_symbol_t* symbols;
};

struct kowhai_protocol_payload_spec_t
{
    uint16_t type;
    uint16_t offset;
    uint16_t size;
};

struct kowhai_protocol_payload_t
{
    struct kowhai_protocol_payload_spec_t spec;
    void* data;
};

struct kowhai_protocol_t
{
    struct kowhai_protocol_header_t header;
    struct kowhai_protocol_payload_t payload;
};

#pragma pack()

#define POPULATE_PROTOCOL_READ(protocol, tree_id_, cmd, symbol_count_, symbols_) \
    {                                                   \
        protocol.header.tree_id = tree_id_;             \
        protocol.header.command = cmd;                  \
        protocol.header.symbol_count = symbol_count_;   \
        protocol.header.symbols = symbols_;             \
    }

#define POPULATE_PROTOCOL_WRITE(protocol, tree_id_, cmd, symbol_count_, symbols_, data_type, data_offset, data_size, data_) \
    {                                                   \
        protocol.header.tree_id = tree_id_;             \
        protocol.header.command = cmd;                  \
        protocol.header.symbol_count = symbol_count_;   \
        protocol.header.symbols = symbols_;             \
        protocol.payload.spec.type = data_type;         \
        protocol.payload.spec.offset = data_offset;     \
        protocol.payload.spec.size = data_size;         \
        protocol.payload.data = data_;                  \
    }

//
// Functions
//

/*
 * Parse a protocol packet and return the tree id
 */
int kowhai_protocol_get_tree_id(void* proto_packet, int packet_size, uint8_t* tree_id);

/*
 * Parse a protocol packet and return the:
 *   - protocol header and payload information
 *   - node memory offset in the tree data
 *   - referenced node
 */
int kowhai_protocol_parse(void* proto_packet, int packet_size, struct kowhai_protocol_t* protocol);

/*
 * Create a protocol packet
 * If function succeeds bytes_required has number of bytes written to proto_packet,
 * if function fails because proto_packet size is too small bytes_required has number of bytes needed
 */
int kowhai_protocol_create(void* proto_packet, int packet_size, struct kowhai_protocol_t* protocol, int* bytes_required);

#endif

