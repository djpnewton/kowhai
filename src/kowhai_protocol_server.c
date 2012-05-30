#include "kowhai_protocol_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void kowhai_server_init_tree_descriptor_sizes(struct kowhai_node_t** descriptors, size_t* sizes, int num)
{
    int i;
    for (i = 0; i < num; i++)
    {
        struct kowhai_node_t* desc = descriptors[i];
        sizes[i] = 0;
        if (desc != NULL)
        {
            int c = 0;
            do
            {
                sizes[i] += sizeof(struct kowhai_node_t);
                if (desc->type == KOW_BRANCH_END)
                    c--;
                else if (desc->type == KOW_BRANCH_START)
                    c++;
                desc++;
            }
            while (c > 0);
        }
    }
}

struct kowhai_tree_t _populate_tree(struct kowhai_protocol_server_t* server, uint16_t tree_id)
{
    struct kowhai_tree_t tree;
    if (tree_id == KOW_UNDEFINED_SYMBOL)
    {
        tree.desc = NULL;
        tree.data = NULL;
    }
    else
    {
        tree.desc = server->tree_descriptors[tree_id];
        tree.data = server->tree_data_buffers[tree_id];
    }
    return tree;
}

void _invalid_tree_id(struct kowhai_protocol_server_t* server, struct kowhai_protocol_t* prot)
{
    int bytes_required;
    printf("    invalid tree id (%d)\n", prot->header.id);
    prot->header.command = KOW_CMD_ERROR_INVALID_TREE_ID;
    kowhai_protocol_create(server->packet_buffer, server->max_packet_size, prot, &bytes_required);
    server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
}

int _get_function_index(struct kowhai_protocol_server_t* server , uint16_t id, int* index)
{
    size_t i = 0;
    while (i < server->function_list_size / sizeof(server->function_list[0]))
    {
        if (server->function_list[i] == id)
        {
            *index = i;
            return 1;
        }
        i++;
    }
    return 0;
}

int kowhai_server_process_packet(struct kowhai_protocol_server_t* server, void* packet, size_t packet_size)
{
    struct kowhai_protocol_t prot;
    int bytes_required, status;

    if (packet_size > server->max_packet_size)
    {
        printf("    error: packet size too large\n");
        return KOW_STATUS_PACKET_BUFFER_TOO_BIG;
    }

    status = kowhai_protocol_parse(packet, packet_size, &prot);
    if (status != KOW_STATUS_OK && status != KOW_STATUS_INVALID_PROTOCOL_COMMAND)
    {
        printf("    ERROR: invalid protocol command\n");
        prot.header.command = KOW_CMD_ERROR_INVALID_COMMAND;
        kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
        server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
        return status;
    }

    switch (prot.header.command)
    {
        case KOW_CMD_WRITE_DATA:
        {
            struct kowhai_tree_t tree;
            printf("    CMD write data\n");
            if (prot.header.id > server->tree_count)
            {
                _invalid_tree_id(server, &prot);
                break;
            }
            // init tree helper struct
            tree = _populate_tree(server, prot.header.id);
            // write to tree
            status = kowhai_write(&tree, prot.payload.spec.data.symbols.count, prot.payload.spec.data.symbols.array_, prot.payload.spec.data.memory.offset, prot.payload.buffer, prot.payload.spec.data.memory.size);
            if (status == KOW_STATUS_OK)
            {
                // call node_written callback
                struct kowhai_node_t* node;
                uint16_t offset;
                kowhai_get_node(tree.desc, prot.payload.spec.data.symbols.count, prot.payload.spec.data.symbols.array_, &offset, &node);
                server->node_written(server, server->node_written_param, node);
                // send response
                prot.header.command = KOW_CMD_WRITE_DATA_ACK;
                kowhai_read(&tree, prot.payload.spec.data.symbols.count, prot.payload.spec.data.symbols.array_, prot.payload.spec.data.memory.offset, prot.payload.buffer, prot.payload.spec.data.memory.size);
                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            }
            else
            {
                switch (status)
                {
                    case KOW_STATUS_INVALID_SYMBOL_PATH:
                        printf("    invalid symbol path\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_SYMBOL_PATH, prot.header.id);
                        break;
                    case KOW_STATUS_INVALID_OFFSET:
                        printf("    invalid payload offset\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_PAYLOAD_OFFSET, prot.header.id);
                        break;
                    case KOW_STATUS_NODE_DATA_TOO_SMALL:
                        printf("    invalid payload size\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_PAYLOAD_SIZE, prot.header.id);
                        break;
                    default:
                        printf("    unkown error\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_UNKNOWN, prot.header.id);
                        break;
                }
                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            }
            break;
        }
        case KOW_CMD_READ_DATA:
        {
            struct kowhai_tree_t tree;
            uint16_t node_offset;
            int size, overhead, max_payload_size;
            struct kowhai_node_t* node;
            struct kowhai_protocol_symbol_spec_t symbols = prot.payload.spec.data.symbols;
            printf("    CMD read data\n");
            if (prot.header.id > server->tree_count)
            {
                _invalid_tree_id(server, &prot);
                break;
            }
            // init tree helper struct
            tree = _populate_tree(server, prot.header.id);
            // get node information
            status = kowhai_get_node(tree.desc, prot.payload.spec.data.symbols.count, prot.payload.spec.data.symbols.array_, &node_offset, &node);
            if (status == KOW_STATUS_OK)
            {
                union kowhai_symbol_t last_sym = symbols.array_[symbols.count-1];
                kowhai_get_node_size(node, &size);
                if (node->count > 1)
                    size = size - size / node->count * last_sym.parts.array_index;
                // get protocol overhead
                prot.header.command = KOW_CMD_READ_DATA_ACK;
                kowhai_protocol_get_overhead(&prot, &overhead);
                // setup max payload size and payload offset
                max_payload_size = server->max_packet_size - overhead;
                prot.payload.spec.data.memory.offset = 0;
                prot.payload.spec.data.memory.type = node->type;
                // allocate payload buffer
                prot.payload.buffer = malloc(server->max_packet_size - overhead);

                // send packets
                while (size > max_payload_size)
                {
                    prot.payload.spec.data.memory.size = max_payload_size;
                    kowhai_read(&tree, prot.payload.spec.data.symbols.count, prot.payload.spec.data.symbols.array_, prot.payload.spec.data.memory.offset, prot.payload.buffer, prot.payload.spec.data.memory.size);
                    kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                    server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
                    // increment payload offset and decrement remaining payload size
                    prot.payload.spec.data.memory.offset += max_payload_size;
                    size -= max_payload_size;
                }
                // send final packet
                prot.header.command = KOW_CMD_READ_DATA_ACK_END;
                prot.payload.spec.data.memory.size = size;
                kowhai_read(&tree, prot.payload.spec.data.symbols.count, prot.payload.spec.data.symbols.array_, prot.payload.spec.data.memory.offset, prot.payload.buffer, prot.payload.spec.data.memory.size);
                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
                // free payload buffer
                free(prot.payload.buffer);
            }
            else
            {
                switch (status)
                {
                    case KOW_STATUS_INVALID_SYMBOL_PATH:
                        printf("    invalid symbol path\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_SYMBOL_PATH, prot.header.id);
                        break;
                    case KOW_STATUS_INVALID_OFFSET:
                        printf("    invalid payload offset\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_PAYLOAD_OFFSET, prot.header.id);
                        break;
                    case KOW_STATUS_NODE_DATA_TOO_SMALL:
                        printf("    invalid payload size\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_PAYLOAD_SIZE, prot.header.id);
                        break;
                    default:
                        printf("    unkown error\n");
                        POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_UNKNOWN, prot.header.id);
                        break;
                }
                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            }
            break;
        }
        case KOW_CMD_READ_DESCRIPTOR:
        {
            struct kowhai_tree_t tree;
            int size, overhead, max_payload_size;
            printf("    CMD read descriptor\n");
            if (prot.header.id > server->tree_count)
            {
                _invalid_tree_id(server, &prot);
                break;
            }
            // init tree helper struct
            tree = _populate_tree(server, prot.header.id);
            // get descriptor size
            size = *(server->tree_descriptor_sizes + prot.header.id);
            // get protocol overhead
            prot.header.command = KOW_CMD_READ_DESCRIPTOR_ACK;
            kowhai_protocol_get_overhead(&prot, &overhead);
            // setup max payload size and payload offset
            max_payload_size = server->max_packet_size - overhead;
            prot.payload.spec.descriptor.offset = 0;
            prot.payload.spec.descriptor.node_count = size / sizeof(struct kowhai_node_t);
            // allocate payload buffer
            prot.payload.buffer = malloc(server->max_packet_size - overhead);

            // send packets
            while (size > max_payload_size)
            {
                prot.payload.spec.descriptor.size = max_payload_size;
                memcpy(prot.payload.buffer, (char*)tree.desc + prot.payload.spec.descriptor.offset, prot.payload.spec.descriptor.size);
                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
                // increment payload offset and decrement remaining payload size
                prot.payload.spec.descriptor.offset += max_payload_size;
                size -= max_payload_size;
            }
            // send final packet
            prot.header.command = KOW_CMD_READ_DESCRIPTOR_ACK_END;
            prot.payload.spec.descriptor.size = size;
            memcpy(prot.payload.buffer, (char*)tree.desc + prot.payload.spec.descriptor.offset, prot.payload.spec.descriptor.size);
            kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
            server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            // free payload buffer
            free(prot.payload.buffer);
            break;
        }
        case KOW_CMD_GET_FUNCTION_LIST:
        {
            int size, overhead, max_payload_size;
            printf("    CMD get function list\n");
            // get function list size
            size = server->function_list_size;
            // get protocol overhead
            prot.header.command = KOW_CMD_GET_FUNCTION_LIST_ACK;
            kowhai_protocol_get_overhead(&prot, &overhead);
            // setup max payload size and payload offset
            max_payload_size = server->max_packet_size - overhead;
            prot.payload.spec.function_list.offset = 0;
            prot.payload.spec.function_list.list_count = size / sizeof(server->function_list[0]);
            // allocate payload buffer
            prot.payload.buffer = malloc(server->max_packet_size - overhead);

            // send packets
            while (size > max_payload_size)
            {
                prot.payload.spec.descriptor.size = max_payload_size;
                memcpy(prot.payload.buffer, (char*)server->function_list + prot.payload.spec.function_list.offset, prot.payload.spec.function_list.size);
                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
                // increment payload offset and decrement remaining payload size
                prot.payload.spec.function_list.offset += max_payload_size;
                size -= max_payload_size;
            }
            // send final packet
            prot.header.command = KOW_CMD_GET_FUNCTION_LIST_ACK_END;
            prot.payload.spec.function_list.size = size;
            memcpy(prot.payload.buffer, (char*)server->function_list + prot.payload.spec.function_list.offset, prot.payload.spec.function_list.size);
            kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
            server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            // free payload buffer
            free(prot.payload.buffer);
            break;
        }
        case KOW_CMD_GET_FUNCTION_DETAILS:
        {
            int i;
            printf("    CMD get function details\n");
            // setup function details
            prot.header.command = KOW_CMD_ERROR_INVALID_FUNCTION_ID;
            if (_get_function_index(server, prot.header.id, &i))
            {
                prot.header.command = KOW_CMD_GET_FUNCTION_DETAILS_ACK;
                prot.payload.spec.function_details = server->function_list_details[i];
            }
            // set payload buffer
            prot.payload.buffer = NULL;

            // send packet
            kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
            server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            break;
        }
        case KOW_CMD_CALL_FUNCTION:
        {
            int function_index, tree_data_size;
            printf("    CMD call function\n");
            // call function
            prot.header.command = KOW_CMD_ERROR_INVALID_FUNCTION_ID;
            if (_get_function_index(server, prot.header.id, &function_index))
            {
                struct kowhai_tree_t tree = _populate_tree(server, server->function_list_details[function_index].tree_in_id);
                if (server->function_list_details[function_index].tree_in_id > server->tree_count &&
                    server->function_list_details[function_index].tree_in_id != KOW_UNDEFINED_SYMBOL)
                {
                    _invalid_tree_id(server, &prot);
                    break;
                }
                tree_data_size = 0;
                if (tree.desc == NULL || kowhai_get_node_size(tree.desc, &tree_data_size) == KOW_STATUS_OK)
                {
                    if (prot.payload.spec.function_call.offset > tree_data_size)
                    {
                        printf("        KOW_CMD_ERROR_INVALID_PAYLOAD_OFFSET\n");
                        prot.header.command = KOW_CMD_ERROR_INVALID_PAYLOAD_OFFSET;
                    }
                    else if (prot.payload.spec.function_call.offset + prot.payload.spec.function_call.size > tree_data_size)
                    {
                        printf("        KOW_CMD_ERROR_INVALID_PAYLOAD_SIZE\n");
                        prot.header.command = KOW_CMD_ERROR_INVALID_PAYLOAD_SIZE;
                    }
                    else
                    {
                        int offset = prot.payload.spec.function_call.offset;
                        int size = prot.payload.spec.function_call.size;
                        printf("        write data (offset: %d, size: %d, tree_data_size: %d)\n", offset, size, tree_data_size);
                        memcpy((char*)tree.data + offset, prot.payload.buffer, size);
                        // setup response details
                        prot.header.command = KOW_CMD_CALL_FUNCTION_ACK;
                        prot.payload.spec.function_call.offset = 0;
                        prot.payload.spec.function_call.size = 0;
                        prot.payload.buffer = NULL;
                        // handle server->function_called when all data has been written
                        if (tree_data_size == 0 || offset + size == tree_data_size)
                        {
                            struct kowhai_tree_t tree = _populate_tree(server, server->function_list_details[function_index].tree_out_id);
                            printf("        function_called callback\n");
                            server->function_called(server, server->function_called_param, prot.header.id);
                            // respond with result tree or not
                            if (tree.desc != NULL)
                            {
                                int size, overhead, max_payload_size;
                                printf("        send return tree\n");
                                prot.header.command = KOW_CMD_CALL_FUNCTION_RESULT;
                                kowhai_protocol_get_overhead(&prot, &overhead);
                                // setup size
                                kowhai_get_node_size(tree.desc, &size);
                                // setup max payload size and payload offset
                                max_payload_size = server->max_packet_size - overhead;
                                prot.payload.spec.function_call.offset = 0;
                                prot.payload.spec.function_call.size = max_payload_size;
                                prot.payload.buffer = tree.data;
                                // send packets
                                while (size > max_payload_size)
                                {
                                    prot.payload.spec.function_call.size = max_payload_size;
                                    prot.payload.buffer = (char*)tree.data + prot.payload.spec.function_call.offset;
                                    kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                                    server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
                                    // increment payload offset and decrement remaining payload size
                                    prot.payload.spec.function_call.offset += max_payload_size;
                                    size -= max_payload_size;
                                }
                                // send final packet
                                prot.header.command = KOW_CMD_CALL_FUNCTION_RESULT_END;
                                prot.payload.spec.function_call.size = size;
                                prot.payload.buffer = (char*)tree.data + prot.payload.spec.function_call.offset;
                                kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
                                server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
                                break;
                            }
                            else
                            {
                                printf("        send no return tree\n");
                                prot.header.command = KOW_CMD_CALL_FUNCTION_RESULT_END;
                            }
                        }
                        else
                        {
                            printf("        send partial function call acknowledge\n");
                        }
                    }
                }
                else
                {
                    _invalid_tree_id(server, &prot);
                    break;
                }
            }
            else
            {
                printf("        cant find function index\n");
            }
            // send packet
            kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
            server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            break;

        }
        default:
            printf("    invalid command (%d)\n", prot.header.command);
            POPULATE_PROTOCOL_CMD(prot, KOW_CMD_ERROR_INVALID_COMMAND, prot.header.id);
            kowhai_protocol_create(server->packet_buffer, server->max_packet_size, &prot, &bytes_required);
            server->send_packet(server, server->send_packet_param, server->packet_buffer, bytes_required);
            break;
    }

    return KOW_STATUS_OK;
}
