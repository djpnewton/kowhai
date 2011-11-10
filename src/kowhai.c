#include "kowhai.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif


/**
 * @brief return the size for a given node type
 * @param type a node type to find the size of
 * @return the size in bytes
 */
int kowhai_get_node_type_size(enum kowhai_node_type type)
{
    switch (type)
    {
		// meta tags only (no real size in the buffer)
		case BRANCH_START:
		case BRANCH_END:
			return 0;

		// normal types to describe a buffer
		case INT8_T:
		case UINT8_T:
			return 1;
		case INT16_T:
		case UINT16_T:
			return 2;
		case INT32_T:
		case UINT32_T:
		case FLOAT_T:
			return 4;
        default:
			#ifdef KOWHAI_DBG
            printf(KOWHAI_ERR" unknown item type: %d\n", type);
			#endif
            return -1;
    }
}


// calculate the complete size of a node including all the sub-elements and array items.
int kowhai_get_node_size(const struct kowhai_node_t *node, int *node_count)
{
	int size = 0;								// size of the node 
	uint16_t i = 0;								// items in the node 
	uint16_t _node_count = 0xFFFF;

	// if node_count is non null then limit the search items
	if (node_count != NULL)
		_node_count = *node_count;

	// if this is not a branch then just return the size of the node item (otherwise we need to drill baby)
	if (node->type != BRANCH_START)
	{
		size = kowhai_get_node_type_size(node[i].type) * node[i].count;
		goto done;
	}
	
	// this is a branch so look through all the elements in this branch and accumulate the sizes
	for (i = 1; i < _node_count; i++)
	{
		switch ((enum kowhai_node_type)node[i].type)
		{
			// navigate the hierarchy info
			case BRANCH_START:
			{
				int new_branch_size = 0;
				int new_branch_items = _node_count - i;	// remaining items
				new_branch_size = kowhai_get_node_size(&node[i], &new_branch_items);
				if (new_branch_size < 0)
					return -2;		// propagate this down the stack
				size += new_branch_size;		// accumulate the branches size
				i += new_branch_items-1;		// skip the already processed branch items
				break;
			}
			case BRANCH_END:
				goto done;
			
			// accumulate the size of all the other node_count
			default:
				size += kowhai_get_node_type_size(node[i].type) * node[i].count;
				break;
		}
	}

	// did we get too many items to check
	if (i >= _node_count)
		return -3;

done:
	if (node_count != NULL)
		*node_count = i+1;
	return size * node[0].count;
	
}


///@brief seek/find a item in the tree given its path
///@param node to start searching from for the given item
///@param path_items number of items in the path (@todo should we just terminate the path instead)
///@param path the path of the item to seek
///@param offset set to number of bytes from the current branch to the item
///@return < 0 on failure
///@todo find the correct index (always 0 atm)
int seek_item(const struct kowhai_node_t *node, int *node_count, int path_items, const union kowhai_path_item *path, uint16_t *offset)
{
	int i;
	uint16_t _offset = 0;
	uint16_t _node_count = 0xFFFF;

	// if node_count is non null then limit the search items
	if (node_count != NULL)
		_node_count = *node_count;

	// look through all the items in the node list
	for (i = 0; i < _node_count; i++)
	{
		uint16_t skip_size;
		int skip_nodes;

		switch ((enum kowhai_node_type)node[i].type)
		{
			case BRANCH_END:
				// if we got a branch end then we didn't find it on this path
				#ifdef KOWHAI_DBG
				printf(KOWHAI_ERR" dead end on branch: %d\n", node[i].symbol);
				#endif
				return -1;
			case BRANCH_START:
			default:
				// if the path symbols match and the item array is large enough to contain our index this could be it ...
				#ifdef KOWHAI_DBG
				printf(KOWHAI_ERR" node.sym = %d, path.sym = %d, node.count = %d path_items = %d\n", node[i].symbol, path->full.symbol, node[i].count, path_items);
				#endif
				if ((path->full.symbol == node[i].symbol) && (node[i].count > path->full.index))
				{
					if (path_items == 1)
					{
						#ifdef KOWHAI_DBG
						printf(KOWHAI_ERR" found symbol: %d\n", node[i].symbol);
						#endif
						// the path fully match in values and length so this is the item we are looking for
						goto done;
					}
					
					if ((enum kowhai_node_type)node[i].type == BRANCH_START)
					{
						// this is not the item but it maybe in this branch so drill baby drill
						#ifdef KOWHAI_DBG
						printf(KOWHAI_ERR" drill on branch: %d\n", node[i].symbol);
						#endif
						uint16_t branch_offset = 0;					// offset into this branch for the item
						int branch_node_count = _node_count - i;	// max nodes to search from here
						int ret = seek_item(&node[i+1], &branch_node_count, path_items - 1, &path[1], &branch_offset);
						if (ret < -1)
							// a error occurred so propagate it out
							return ret;
						if (ret == -1)
							// branch ended without finding our item so continue to next item
							break;
						// we found the item in this branch so update _offset and i
						_offset += branch_offset;
						i += branch_node_count - 1;
						goto done;
					}
				}
				break;
		}
		
		// this item is not a match so skip it (find out how many bytes and nodes to skip)
		#ifdef KOWHAI_DBG
		printf(KOWHAI_ERR" doing skip on symbol: %d\n", node[i].symbol);
		#endif
		skip_nodes = _node_count - i; // max nodes left
		skip_size = kowhai_get_node_size(&node[i], &skip_nodes);
		#ifdef KOWHAI_DBG
		printf(KOWHAI_ERR" skipping %d nodes and %d bytes on symbol: %d\n", skip_nodes, skip_size, node[i].symbol);
		#endif
		_offset += skip_size;
		i += skip_nodes - 1;
	}

done:
	if (node_count != NULL)
		*node_count = i + 1;
	if (offset != NULL)
		*offset = _offset;
	return _offset;	// success
}


///@brief seek/find a item in the tree given its path
///@param tree to be searched for a item
///@param path_items number of items in the path (@todo should we just terminate the path instead)
///@param path the path of the item to seek
///@param offset into the tree buffer
#if 0
static int _get_setting_offset(struct kowhai_node_t** tree, int num_symbols, union kowhai_symbol_t* symbols, int symbols_matched, int* finished, int* finished_on_leaf)
{
    int offset = 0;
    do
    {
#ifdef KOWHAI_DBG
        printf(KOWHAI_INFO" (*tree)->type: %d, (*tree)->symbol: %d, (*tree)->count: %d\n", (*tree)->type, (*tree)->symbol, (*tree)->count);
#endif

        // match symbols here
        if (symbols_matched < num_symbols)
        {
            if (symbols[symbols_matched].parts.name == (*tree)->symbol)
            {
                symbols_matched++;
#ifdef KOWHAI_DBG
                printf(KOWHAI_INFO" symbol match, (*tree)->symbol: %d, symbols_matched: %d\n", (*tree)->symbol, symbols_matched);
#endif
            }
        }

        // return offset if we have matched the symbols
        if (symbols_matched == num_symbols)
        {
#ifdef KOWHAI_DBG
            printf(KOWHAI_INFO" return offset: %d\n", offset);
#endif
            *finished = 1;
            *finished_on_leaf = (*tree)->type == NODE_TYPE_LEAF;
            return offset;
        }
        
        switch ((*tree)->type)
        {
            case NODE_TYPE_BRANCH:
            {
                // recurse into branch
                struct kowhai_node_t* branch = *tree;
                int temp;
                (*tree)++;
                temp = _get_setting_offset(tree, num_symbols, symbols, symbols_matched, finished, finished_on_leaf);
                if (temp == -1)
                {
#ifdef KOWHAI_DBG
                    printf(KOWHAI_ERR" branch parse failed, node symbol: %d, current offset: %d\n", branch->symbol, offset);
#endif
                    return -1;
                }
                if (!(*finished))
                    temp *= branch->count;
                else
                {
                    // branch array
                    uint16_t array_index = symbols[symbols_matched - 1].parts.array_index;
                    if (array_index > 0)
                    {
                        int __steps = 0;
                        if (array_index > branch->count - 1)
                        {
#ifdef KOWHAI_DBG
                            printf(KOWHAI_ERR" symbol match error, symbol array_index: %d, node count: %d\n", array_index, branch->count);
#endif
                            return -1;
                        }
                        temp += _get_branch_size(branch + 1, &__steps) * array_index;
                    }
                    // leaf array
                    if (*finished_on_leaf)
                    {
                        *finished_on_leaf = 0;
                        array_index = symbols[symbols_matched].parts.array_index;
                        if (array_index > 0)
                        {
                            struct kowhai_node_t* leaf = *tree;
                            if (leaf->type != NODE_TYPE_LEAF)
                                return -1;
                            if (array_index > leaf->count - 1)
                                return -1;
                            if (leaf->param1 != LEAF_TYPE_SETTING)
                                return -1;
                            temp += kowhai_get_setting_size(leaf->param2) * array_index;
                        }
                    }
                }
                offset += temp;

                // return offset if we have matched the symbols
                if (*finished)
                    return offset;

                break;
            }
            case NODE_TYPE_END:
                // return out of branch
                return offset;
            case NODE_TYPE_LEAF:
                // append leaf settings to offset
                if ((*tree)->param1 == LEAF_TYPE_SETTING)
                {
                    int temp = kowhai_get_setting_size((*tree)->param2);
                    offset += temp * (*tree)->count;
                }
                break;
            default:
#ifdef KOWHAI_DBG
                printf(KOWHAI_ERR" unknown node type %d\n", (*tree)->type);
#endif
                return -1;
        }

        // increment tree node pointer
        (*tree)++;
    }
    while (1);
}

/* TODO, this function should take a tree count for safety */
int kowhai_get_setting(struct kowhai_node_t* tree, int num_symbols, union kowhai_symbol_t* symbols, int* offset, struct kowhai_node_t** target_node)
{
    struct kowhai_node_t* node = tree;
    int finished = 0, finished_on_leaf = 0;
    int _offset = _get_setting_offset(&node, num_symbols, symbols, 0, &finished, &finished_on_leaf);
    if (_offset > -1)
    {
        *offset = _offset;
        *target_node = node;
        return 1;
    }
    return 0;
}

int _get_node_size(struct kowhai_node_t* node)
{
    switch (node->type)
    {
        case NODE_TYPE_BRANCH:
        {
            int branch_size;
            if (kowhai_get_branch_size(node, &branch_size))
                return branch_size;
            else
                return -1;
            break;
        }
        case NODE_TYPE_LEAF:
            if (node->param1 == LEAF_TYPE_SETTING)
            {
                int leaf_size = kowhai_get_setting_size(node->param2);
                leaf_size *= node->count;
                return leaf_size;
            }
            else
                return -1;
            break;
        default:
            return -1;
    }
}

int kowhai_read(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, void* result, int read_size)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (read_size > _get_node_size(node))
        return 0;
    memcpy(result, (char*)settings_buffer + offset, read_size);
    return 1;
}

int kowhai_write(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, void* value, int write_size)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (write_size > _get_node_size(node))
        return 0;
    memcpy((char*)settings_buffer + offset, value, write_size);
    return 1;
}

int kowhai_get_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, char* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_CHAR || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UCHAR))
    {
        *result = *((char*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_int16(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int16_t* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT16 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT16))
    {
        *result = *((int16_t*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_int32(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int32_t* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT32 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT32))
    {
        *result = *((uint32_t*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_get_float(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, float* result)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_FLOAT)
    {
        *result = *((float*)((char*)settings_buffer + offset));
        return 1;
    }
    return 0;
}

int kowhai_set_char(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, char value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_CHAR || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UCHAR))
    {
        char* target_address = (char*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_int16(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int16_t value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT16 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT16))
    {
        int16_t* target_address = (int16_t*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_int32(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, int32_t value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        (KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_INT32 || KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_UINT32))
    {
        uint32_t* target_address = (uint32_t*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}

int kowhai_set_float(struct kowhai_node_t* tree, void* settings_buffer, int num_symbols, union kowhai_symbol_t* symbols, float value)
{
    struct kowhai_node_t* node;
    int offset;
    if (!kowhai_get_setting(tree, num_symbols, symbols, &offset, &node))
        return 0;
    if (node->type == NODE_TYPE_LEAF && node->param1 == LEAF_TYPE_SETTING &&
        KOWHAI_RAW_SETTING_TYPE(node->param2) == SETTING_TYPE_FLOAT)
    {
        float* target_address = (float*)((char*)settings_buffer + offset);
        *target_address = value;
        return 1;
    }
    return 0;
}
#endif
