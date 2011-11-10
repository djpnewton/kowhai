#ifndef _KOWHAI_PROTOCOL_H_
#define _KOWHAI_PROTOCOL_H_

#include "kowhai.h" 

//
// Protocol commands
//

// Write a setting
#define CMD_WRITE_SETTING        0x00
#define CMD_WRITE_SETTING_SHADOW 0x01
// Acknowledge write setting command
#define CMD_WRITE_SETTING_ACK    0x0F
// Read a setting
#define CMD_READ_SETTING         0x10
#define CMD_READ_SETTING_SHADOW  0x11
// Acknowledge read setting command (and return setting contents)
#define CMD_READ_SETTING_ACK     0x1F
// Acknowledge read setting command (this is the final packet)
#define CMD_READ_SETTING_ACK_END 0x1E
// Trigger an action
#define CMD_TRIGGER_ACTION       0x20
// Acknowledge trigger action command
#define CMD_TRIGGER_ACTION_ACK   0x2F
// Read the tree
#define CMD_READ_TREE            0xF0
// Acknowledge read tree command (and return tree contents)
#define CMD_READ_TREE_ACK        0xFF
// Acknowledge read tree command (this is the final packet)
#define CMD_READ_TREE_ACK_END    0xFE

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

