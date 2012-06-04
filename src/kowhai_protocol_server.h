#ifndef _KOWHAI_PROTOCOL_SERVER_H_
#define _KOWHAI_PROTOCOL_SERVER_H_

#include "kowhai_protocol.h" 

#include <stddef.h>

typedef struct kowhai_protocol_server_t* pkowhai_protocol_server_t;

/**
 * @brief callback used to send a kowhai packet to the indented target
 * @param server the protocol server object
 * @param param application specific parameter passed through
 * @param packet the packet buffer to write out
 * @param packet_size bytes in the packet buffer
 */
typedef void (*kowhai_send_packet_t)(pkowhai_protocol_server_t server, void* param, void* packet, size_t packet_size);

/**
 * @brief called after a node has been written over the kowhai protocol
 * @param server the protocol server object
 * @param param application specific parameter passed through
 * @param node points to the node that was updated
 */
typedef void (*kowhai_node_written_t)(pkowhai_protocol_server_t server, void* param, struct kowhai_node_t* node);

/**
 * @brief called after a function has been called over the kowhai protocol
 * @param server the protocol server object
 * @param param application specific parameter passed through
 * @param function_id the id of the function that was called
 */
typedef void (*kowhai_function_called_t)(pkowhai_protocol_server_t server, void* param, uint16_t function_id);

struct kowhai_protocol_server_t
{
    size_t max_packet_size;
    void* packet_buffer;
    kowhai_node_written_t node_written;
    void* node_written_param;
    kowhai_send_packet_t send_packet;
    void* send_packet_param;
    int tree_list_count;
    uint16_t* tree_list;
    struct kowhai_node_t** tree_descriptors;
    size_t* tree_descriptor_sizes;
    void** tree_data_buffers;
    int function_list_count;
    uint16_t* function_list;
    struct kowhai_protocol_function_details_t* function_list_details;
    kowhai_function_called_t function_called;
    void* function_called_param;
};

void kowhai_server_init_tree_descriptor_sizes(struct kowhai_node_t** descriptors, size_t* sizes, int num);

/**
 * @brief Parse a kowhai packet and perform requested commands
 * @param server configuration for this server
 * @param packet parse this and perform commands
 * @param packet_size number bytes in packet
 */
int kowhai_server_process_packet(struct kowhai_protocol_server_t* server, void* packet, size_t packet_size);




#endif
