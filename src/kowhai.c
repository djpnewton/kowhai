#include "kowhai.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif


int kowhai_get_node_type_size(uint16_t type)
{
    switch ((enum kowhai_node_type_t)type)
    {
        // meta tags only (no real size in the buffer)
        case KOW_BRANCH_START:
        case KOW_BRANCH_END:
            return 0;

        // normal types to describe a buffer
        case KOW_INT8:
        case KOW_UINT8:
            return 1;
        case KOW_INT16:
        case KOW_UINT16:
            return 2;
        case KOW_INT32:
        case KOW_UINT32:
        case KOW_FLOAT:
            return 4;
        default:
#ifdef KOWHAI_DBG
            printf(KOWHAI_ERR" unknown item type: %d\n", type);
#endif
            return -1;
    }
}

// calculate the complete size of a node including all the sub-elements and array items.
static int get_node_size(const struct kowhai_node_t *node, int *size, int *num_nodes_processed)
{
    int _size = 0;
    uint16_t i = 0;

    *num_nodes_processed = 0;

    // if this is not a branch then just return the size of the node item (otherwise we need to drill baby)
    if (node->type != KOW_BRANCH_START)
    {
        _size = kowhai_get_node_type_size(node->type) * node->count;
        *num_nodes_processed = 1;
        goto done;
    }
    
    // this is a branch so look through all the elements in this branch and accumulate the sizes
    while (1)
    {
        i++;
        *num_nodes_processed = i;
        switch ((enum kowhai_node_type_t)node[i].type)
        {
            // navigate the hierarchy info
            case KOW_BRANCH_START:
            {
                int child_branch_size = 0;
                int _num_child_nodes_processed;
                int ret;
                ret = get_node_size(node + i, &child_branch_size, &_num_child_nodes_processed);
                if (ret != KOW_STATUS_OK)
                    return ret;
                // accumulate the branches size
                _size += child_branch_size;
                // skip the already processed nodes
                i += _num_child_nodes_processed;
                break;
            }
            case KOW_BRANCH_END:
                // accumulate the whole array
                _size *= node->count;
                goto done;
            
            // accumulate the size of all the other node_count
            default:
                _size += kowhai_get_node_type_size(node[i].type) * node[i].count;
                break;
        }
    }

done:
    if (size != NULL)
        *size = _size;

    return KOW_STATUS_OK;
}

int kowhai_get_node_size(const struct kowhai_node_t *node, int *size)
{
    int num_nodes_processed;
    return get_node_size(node, size, &num_nodes_processed);
}

static int get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node, int* permissions, int initial_branch)
{
    int i = 0;
    uint16_t _offset = 0;
    int _perms = 0;
    int ret;

    // look through all the items in the node list
    while (1)
    {
        int skip_size;
        int skip_nodes;

        switch ((enum kowhai_node_type_t)node[i].type)
        {
            case KOW_BRANCH_END:
                // if we got a branch end then we didn't find it on this path
                return KOW_STATUS_INVALID_SYMBOL_PATH;
            case KOW_BRANCH_START:
            default:
                // if the path symbols match and the node array count is large enough to contain our index this could be the target node
                if ((symbols->parts.name == node[i].symbol) && (node[i].count > symbols->parts.array_index))
                {
                    // add the permissions of this node
                    _perms |= node[i].permissions;

                    if (num_symbols == 1)
                    {
                        // the symbol paths fully match in values and length so this is the node we are looking for
                        ret = KOW_STATUS_OK;
                        if (target_node != NULL)
                            *target_node = (struct kowhai_node_t*)node + i;
                        goto done;
                    }
                    
                    if ((enum kowhai_node_type_t)node[i].type == KOW_BRANCH_START)
                    {
                        uint16_t branch_offset = 0;
                        int branch_permissions;
                        // this is not the target node but it is possibly in this branch so drill baby drill
                        ret = get_node(node + i + 1, num_symbols - 1, symbols + 1, &branch_offset, target_node, &branch_permissions, 0);
                        if (ret == KOW_STATUS_INVALID_SYMBOL_PATH)
                            // branch ended without finding our target node so goto next
                            break;
                        // add the branch offset to current total
                        _offset += branch_offset;
                        // add the branch permissions to current collection
                        _perms |= branch_permissions;
                        goto done;
                    }
                }
                break;
        }
        
        if (initial_branch)
            return KOW_STATUS_INVALID_SYMBOL_PATH;
        // this item is not a match so skip it (find out how many bytes and nodes to skip)
        ret = get_node_size(node + i, &skip_size, &skip_nodes);
        if (ret != KOW_STATUS_OK)
            // propagate the error
            return ret;
        _offset += skip_size;
        if (node[i].type == KOW_BRANCH_START)
            i++;
        i += skip_nodes;
    }

done:

    // update offset return parameter
    if (offset != NULL)
    {
        // we have the index of the target node
        // the offset is: (the size of this node / its count) * symbol array index
        int size;
        ret = kowhai_get_node_size(node + i, &size);
        if (ret != KOW_STATUS_OK)
            return ret;
        _offset += (size / node[i].count) * symbols->parts.array_index;
        *offset = _offset;
    }

    // update permissions return parameter
    if (permissions != NULL)
        *permissions = _perms;

    return ret;
}

int kowhai_get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node)
{
    return kowhai_get_node2(node, num_symbols, symbols, offset, target_node, NULL);
}

int kowhai_get_node2(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node, int* permissions)
{
    if (node->type != KOW_BRANCH_START)
        return KOW_STATUS_INVALID_DESCRIPTOR;
    return get_node(node, num_symbols, symbols, offset, target_node, permissions, 1);
}

int kowhai_read(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int read_offset, void* result, int read_size)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status, size;

    // find this node
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_WRITE_ONLY) == KOW_WRITE_ONLY)
        return KOW_STATUS_NODE_WRITE_ONLY;
    if (read_offset < 0)
        return KOW_STATUS_INVALID_OFFSET;

    // check the read wont overrun the item
    status = kowhai_get_node_size(node, &size);
    if (status != KOW_STATUS_OK)
        return status;
    if (read_size + read_offset > size)
        return KOW_STATUS_NODE_DATA_TOO_SMALL;

    // do read
    memcpy(result, (char*)tree_data + offset + read_offset, read_size);
    return status;
}

int kowhai_write(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int write_offset, void* value, int write_size)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status, size;
    
    // find this node
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_READ_ONLY) == KOW_READ_ONLY)
        return KOW_STATUS_NODE_READ_ONLY;
    if (write_offset < 0)
        return KOW_STATUS_INVALID_OFFSET;
    
    // check the write wont overrun the item
    status = kowhai_get_node_size(node, &size);
    if (status != KOW_STATUS_OK)
        return status;
    if (write_size + write_offset > size)
        return KOW_STATUS_NODE_DATA_TOO_SMALL;
    
    // do write
    memcpy((char*)tree_data + offset + write_offset, value, write_size);
    return status;
}

int kowhai_get_char(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, char* result)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_WRITE_ONLY) == KOW_WRITE_ONLY)
        return KOW_STATUS_NODE_WRITE_ONLY;
    if (node->type == KOW_INT8 || node->type == KOW_UINT8)
    {
        *result = *((char*)((char*)tree_data + offset));
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_get_int16(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int16_t* result)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_WRITE_ONLY) == KOW_WRITE_ONLY)
        return KOW_STATUS_NODE_WRITE_ONLY;
    if (node->type == KOW_INT16 || node->type == KOW_UINT16)
    {
        *result = *((int16_t*)((char*)tree_data + offset));
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_get_int32(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int32_t* result)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_WRITE_ONLY) == KOW_WRITE_ONLY)
        return KOW_STATUS_NODE_WRITE_ONLY;
    if (node->type == KOW_INT32 || node->type == KOW_UINT32)
    {
        *result = *((uint32_t*)((char*)tree_data + offset));
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_get_float(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, float* result)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_WRITE_ONLY) == KOW_WRITE_ONLY)
        return KOW_STATUS_NODE_WRITE_ONLY;
    if (node->type == KOW_FLOAT)
    {
        *result = *((float*)((char*)tree_data + offset));
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_set_char(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, char value)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_READ_ONLY) == KOW_READ_ONLY)
        return KOW_STATUS_NODE_READ_ONLY;
    if (node->type == KOW_INT8 || node->type == KOW_UINT8)
    {
        char* target_address = (char*)((char*)tree_data + offset);
        *target_address = value;
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_set_int16(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int16_t value)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_READ_ONLY) == KOW_READ_ONLY)
        return KOW_STATUS_NODE_READ_ONLY;
    if (node->type == KOW_INT16 || node->type == KOW_UINT16)
    {
        int16_t* target_address = (int16_t*)((char*)tree_data + offset);
        *target_address = value;
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_set_int32(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, int32_t value)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_READ_ONLY) == KOW_READ_ONLY)
        return KOW_STATUS_NODE_READ_ONLY;
    if (node->type == KOW_INT32 || node->type == KOW_UINT32)
    {
        uint32_t* target_address = (uint32_t*)((char*)tree_data + offset);
        *target_address = value;
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

int kowhai_set_float(struct kowhai_node_t* tree_descriptor, void* tree_data, int num_symbols, union kowhai_symbol_t* symbols, float value)
{
    struct kowhai_node_t* node;
    uint16_t offset;
    int permissions, status;
    status = kowhai_get_node2(tree_descriptor, num_symbols, symbols, &offset, &node, &permissions);
    if (status != KOW_STATUS_OK)
        return status;
    if ((permissions & KOW_READ_ONLY) == KOW_READ_ONLY)
        return KOW_STATUS_NODE_READ_ONLY;
   if (node->type == KOW_FLOAT)
    {
        float* target_address = (float*)((char*)tree_data + offset);
        *target_address = value;
        return status;
    }
    return KOW_STATUS_INVALID_NODE_TYPE;
}

