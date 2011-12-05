#include "kowhai_utils.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif

// forward decalre this to bring it in from kowhai.c as we need it
///@todo there should be a way to either use the public api or get at internal kowhia stuff more easily
int get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node, int initial_branch);

/**
 * @brief 
 * @param dst, branch start to search for matches from
 * @param src, matching src branch start at the current dst level
 */
static int merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src, int depth)
{
	int ret;
	uint16_t offset;
	struct kowhai_node_t *target_node;
	union kowhai_symbol_t symbol[1];
	int size;
	unsigned int i;
	unsigned int skip_nodes;

	// go through all the dst nodes and look for matches in src
	while (1)
	{
		// are we at the end of this branch, if so branch is merged
		if (dst->desc->type == KOW_BRANCH_END)
			return KOW_STATUS_OK;

		// does the dst path exist in the src path
		skip_nodes = 0;
		symbol[0].parts.name = dst->desc->symbol;
		symbol[0].parts.array_index = 0;
		ret = get_node(src->desc, 1, symbol, &offset, &target_node, 0);
		switch (ret)
		{
			case KOW_STATUS_OK:
				// found matching node in src
				// if the array counts match in src and dst then merge the arrays, otherwise skip it
				///@todo we could just merge the array if the dst has enough room, or merge as much as we have space for ?
				if (dst->desc->count == target_node->count)
				{
					if (dst->desc->type == KOW_BRANCH_START)
					{
						// merge each branch array item one by one
						struct kowhai_tree_t _dst;
						struct kowhai_tree_t _src;
						_dst = *dst;
						for (i = 0; i < dst->desc->count; i++)
						{
							// get the offset into src for the branch array item to update
							symbol[0].parts.array_index = i;
							ret = get_node(src->desc, 1, symbol, &offset, NULL, 0);
							if (ret != KOW_STATUS_OK)
								return ret;
							_src.desc = &target_node[1];
							_src.data = ((uint8_t *)src->data + offset);
							_dst.desc = dst->desc + 1;
							// merge this branch array item (drill)
							ret = merge(&_dst, &_src, depth + 1);
							if (ret != KOW_STATUS_OK)
								return ret;
						}
						// find the number of nodes to skip (don't skip them now as we need to point at the branch_start to find 
						// its size below) we will skip it after that
						skip_nodes = ((unsigned int)_dst.desc - (unsigned int)dst->desc) / sizeof(struct kowhai_node_t);
					}
					else
					{
						// only merge nodes that have the same type
						///@todo we may want to allow up casting eg from uint16_t to uint32_t
						if (dst->desc->type == target_node->type)
						{
							// since this is not a branch so we can just copy the whole array over
							int ret = kowhai_get_node_size(dst->desc, &size);
							if (ret != KOW_STATUS_OK)
								// propagate error 
								return ret;
							///@todo perhaps this should use kohai_read/write here for safety ??
							memcpy(dst->data, ((uint8_t *)src->data + offset), size);
						}

					}
				}
				break;
			case KOW_STATUS_INVALID_SYMBOL_PATH:
				// could not find matching node in src so leave this nodes value alone and move on to next node
				break;
			default:
				// propagate error down
				return ret;
		}

		// done with this node increment our position in the dst tree
		// note if this was a branch skip_nodes will != 0 and contain the number of nodes and
		// kowhai_get_node_size will give the size of the whole branch
		ret = kowhai_get_node_size(dst->desc, &size);
		if (ret != KOW_STATUS_OK)
			return ret;
		dst->data = (uint8_t *)dst->data + size;
		dst->desc += skip_nodes + 1;	
		
		// if this tree is not nicely formed (wrapped in branch start/end) then the next item may not be a 
		// branch end, instead we might just run off the end of the buffer so force a stop
		if (depth == 0)
			return KOW_STATUS_OK;

	}
}

// merge nodes that are common to src and dst from src into dst leaving unique nodes unchanged
int kowhai_merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src)
{
	struct kowhai_tree_t *_dst = dst;

	// we only support merging of well formed tree's (both src and dst)
	if (dst->desc->type != KOW_BRANCH_START ||
		src->desc->type != KOW_BRANCH_START)
		return KOW_STATUS_INVALID_DESCRIPTOR;
	
	// update all the notes in dst that are common to dst and src
	return merge(_dst, src, 0);
}

