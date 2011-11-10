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
// Functions
//

/*
 * Parse a protocol packet and return the:
 *   - tree id
 *   - tree command
 *   - referenced node
 *   - node memory offset in the tree
 *   - payload offset (if reading or writing a setting, or reading the tree)
 *   - payload (if reading or writing a setting, or reading the tree)
 */
int kowhai_protocol_parse(void* proto_packet, int packet_size, uint8_t* tree_id, uint8_t* cmd, struct kowhai_node_t** node, int* node_offset, int* payload_offset, void** payload);

#endif

