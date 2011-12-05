#include "kowhai_utils.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_UTILS_ERR "KOWHAI_UTILS ERROR:"
#define KOWHAI_UTILS_INFO "KOWHAI_UTILS INFO: "
#define KOWHAI_TABS "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
#endif

// forward decalre this to bring it in from kowhai.c as we need it
///@todo there should be a way to either use the public api or get at internal kowhia stuff more easily
int get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node, int initial_branch);

typedef int (*on_diff_t)(const struct kowhai_tree_t *left, const struct kowhai_tree_t *right, int depth);

/**
 * @brief diff left and right tree's
 * @param left, diff this tree against right
 * @param right, diff this tree against left
 * @param on_diff, call this when a difference is found
 * @param depth, how deep in the tree are we (0 root, 1 first branch, etc)
 */
static int diff(struct kowhai_tree_t *left, struct kowhai_tree_t *right, on_diff_t on_diff, int depth)
{
	int ret;
	uint16_t offset;
	struct kowhai_node_t *target_node;
	union kowhai_symbol_t symbol[1];
	int size;
	unsigned int i;
	unsigned int skip_nodes;
	struct kowhai_tree_t _left;
	struct kowhai_tree_t _right;

	// go through all the left nodes and look for matches in right 
	while (1)
	{
		// are we at the end of this branch, if so branch has been diff'ed completely
		if (left->desc->type == KOW_BRANCH_END)
		{
			///@todo we have found all the common nodes between left and right, and all the unique nodes
			/// in left but now we just have to find all the unique nodes in the right branch
			#ifdef KOWHAI_DBG
			printf(KOWHAI_UTILS_INFO "(%d)%.*s pop\n", depth, depth, KOWHAI_TABS, left->desc->symbol);
			#endif
			return KOW_STATUS_OK;
		}

		// does the left path exist in the right path
		skip_nodes = 0;
		symbol[0].parts.name = left->desc->symbol;
		symbol[0].parts.array_index = 0;
		ret = get_node(right->desc, 1, symbol, &offset, &target_node, 0);
		switch (ret)
		{
			case KOW_STATUS_OK:
				// found matching node in right 
				if (left->desc->type == KOW_BRANCH_START)
				{
					// if the array counts match in left and right then diff the arrays, otherwise the arrays are unique
					///@todo we could just diff all the items in the array if we want to allow array sizes to change 
					if (left->desc->count == target_node->count)
					{
						// diff each branch array item one by one
						_left = *left;
						///@todo if allowing changing array sizes then, for (i = 0; i < min(left count, right count)...
						for (i = 0; i < left->desc->count; i++)
						{
							// get the offset into right for the branch array item to update
							symbol[0].parts.array_index = i;
							ret = get_node(right->desc, 1, symbol, &offset, NULL, 0);
							if (ret != KOW_STATUS_OK)
								return ret;
							_right.desc = &target_node[1];
							_right.data = ((uint8_t *)right->data + offset);
							_left.desc = left->desc + 1;
							// diff this branch array item (drill)
							#ifdef KOWHAI_DBG
							printf(KOWHAI_UTILS_INFO "(%d)%.*s drill\n", depth, depth, KOWHAI_TABS, left->desc->symbol);
							#endif
							ret = diff(&_left, &_right, on_diff, depth + 1);
							if (ret != KOW_STATUS_OK)
								return ret;
						}
						// find the number of nodes to skip (don't skip them now as we need to point at the branch_start to find 
						// its size below) we will skip it after that
						skip_nodes = ((unsigned int)_left.desc - (unsigned int)left->desc) / sizeof(struct kowhai_node_t);
					}
					else if (on_diff != NULL)
					{
						// array sizes don't match so both are unique
						ret = on_diff(left, NULL, depth);
						if (ret != KOW_STATUS_OK)
							return ret;
					}
				}
				else
				{
					// if the array counts match in left and right then diff the arrays, otherwise the arrays are unique
					///@todo we could just diff all the items in the array if we want to allow array sizes to change 
					if (left->desc->count == target_node->count)
					{
						// if nodes have the same type then diff them, else they are unique
						///@todo we may want to allow up casting eg from uint16_t to uint32_t
						if (left->desc->type == target_node->type)
						{
							// since this is not a branch so we can just copy the whole array over
							int ret = kowhai_get_node_size(left->desc, &size);
							if (ret != KOW_STATUS_OK)
								// propagate error 
								return ret;
							///@todo if allowing changing array sizes then, if right size is less than left then limit to right size
							///@todo perhaps this should use kohai_read/write here for safety ??
							if ((memcpy(left->data, ((uint8_t *)right->data + offset), size) != 0) && (on_diff != NULL))
							{
								// nodes differ from left to right so run on_diff event
								_right.desc = target_node;
								_right.data = right->data + offset;
								ret = on_diff(left, &_right, depth);
								if (ret != KOW_STATUS_OK)
									return ret;
							}

						}
					}
					else if (on_diff != NULL)
					{
						// array sizes don't match so both are unique
						ret = on_diff(left, NULL, depth);
						if (ret != KOW_STATUS_OK)
							return ret;
						ret = on_diff(NULL, right, depth);
						if (ret != KOW_STATUS_OK)
							return ret;
					}
				}
				break;
			case KOW_STATUS_INVALID_SYMBOL_PATH:
				// could not find matching node in right so this node is unique to the left tree
				if (on_diff != NULL)
				{
					ret = on_diff(left, NULL, depth);
					if (ret != KOW_STATUS_OK)
						return ret;
				}
				break;
			default:
				// propagate error down
				return ret;
		}

		// done with this node increment our position in the left tree
		// note if this was a branch skip_nodes will != 0 and contain the number of nodes and
		// kowhai_get_node_size will give the size of the whole branch
		ret = kowhai_get_node_size(left->desc, &size);
		if (ret != KOW_STATUS_OK)
			return ret;
		left->data = (uint8_t *)left->data + size;
		left->desc += skip_nodes + 1;	
		
		// if this tree is not nicely formed (wrapped in branch start/end) then the next item may not be a 
		// branch end, instead we might just run off the end of the buffer so force a stop
		if (depth == 0)
			return KOW_STATUS_OK;

	}
}

/**
 * @brief called by diff when merging 
 * @param dst this is the destination node to merge common source nodes into, or NULL if node is unique to src
 * @param src this is the source node to merge into common destination nodes, or NULL if node is unique to dst
 * @param depth, how deep in the tree are we (0 root, 1 first branch, etc)
 */
int on_diff_merge(const struct kowhai_tree_t *dst, const struct kowhai_tree_t *src, int depth)
{
	int ret;
	int size;
	uint8_t *dst_data;
	
	if (dst == NULL)
	{
		#ifdef KOWHAI_DBG
		printf(KOWHAI_UTILS_INFO "(%d)%.*s unique node %d in src\n", depth, depth, KOWHAI_TABS, src->desc->symbol);
		#endif
		return KOW_STATUS_OK;
	}

	if (src == NULL)
	{
		#ifdef KOWHAI_DBG
		printf(KOWHAI_UTILS_INFO "(%d)%.*s unique node %d in dst\n", depth, depth, KOWHAI_TABS, dst->desc->symbol);
		#endif
		return KOW_STATUS_OK;
	}

	// both nodes exist but differ so copy src into dst
	ret = kowhai_get_node_size(dst->desc, &size);
	if (ret != KOW_STATUS_OK)
		// propagate error 
		return ret;
	#ifdef KOWHAI_DBG
	printf(KOWHAI_UTILS_INFO "(%d)%.*s merging %d from src into dst\n", depth, depth, KOWHAI_TABS, dst->desc->symbol);
	#endif
	dst_data = dst->data;
	memcpy(dst_data, src->data, size);
	
	return KOW_STATUS_OK;
}

// merge nodes that are common to src and dst from src into dst leaving unique nodes unchanged
int kowhai_merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src)
{
	struct kowhai_tree_t *_dst = dst;

	#ifdef KOWHAI_DBG
	printf("\n");
	#endif
	// we only support merging of well formed tree's (both src and dst)
	if (dst->desc->type != KOW_BRANCH_START ||
		src->desc->type != KOW_BRANCH_START)
		return KOW_STATUS_INVALID_DESCRIPTOR;
	
	// update all the notes in dst that are common to dst and src
	return diff(_dst, src, on_diff_merge, 0);
}

