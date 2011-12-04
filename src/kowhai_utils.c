#include "kowhai_utils.h"

#include <string.h>

#ifdef KOWHAI_DBG
#include <stdio.h>
#define KOWHAI_ERR "KOWHAI ERROR:"
#define KOWHAI_INFO "KOWHAI INFO: "
#endif

static int merge_nodes(struct kowhai_tree_t *dst, struct kowhai_tree_t *src)
{
	// only merge nodes that have the same type and size
	///@todo we may want to allow up casting eg from uint16_t to uint32_t and smaller arrays to merge into bigger ones ?
	if (dst->desc->type == src->desc->type &&
		dst->desc->count == src->desc->count)
	{
		int size;
		int ret = kowhai_get_node_size(dst->desc, &size);
		if (ret != KOW_STATUS_OK)
			return ret;
		///@todo perhaps this should use kohai_read/write here for safety ??
		memcpy(dst->data, src->data, size);
		return KOW_STATUS_OK;
	}
	return KOW_STATUS_INVALID_MERGE_NODE_TYPE;
}

// forward decalre this to bring it in from kowhai.c as we need it
///@todo there should be a way to either use the public api or get at internal kowhia stuff more easily
int get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node, int initial_branch);

/**
 * @brief 
 * @param dst, branch start to search for matches from
 * @param src, matching src branch start at the current dst level
 */
static int merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src)
{
	int ret;
	uint16_t offset;
	struct kowhai_node_t *target_node;
	union kowhai_symbol_t symbol[1];
	int size;

	// go through all the dst nodes and look for matches in src
	while (1)
	{
		if (dst->desc->type == KOW_BRANCH_END)
			// dst branch ended so we are done
			return KOW_STATUS_OK;
		
		// try to find this node in the current src branch
		symbol[0].symbol = dst->desc->symbol;
		ret = get_node(src->desc, 1, symbol, &offset, &target_node, 0);
		switch (ret)
		{
			case KOW_STATUS_OK:
				// found matching node in src
				if (dst->desc->type == KOW_BRANCH_START)
				{
					// found matching branches in src and dst so merge these branches (drill)
					struct kowhai_tree_t _src = {&target_node[1], ((uint8_t *)src->data + offset)};
					dst->desc++;
					ret = merge(dst, &_src);
					if (ret != KOW_STATUS_OK)
						return ret;

					// skip this branch since its already been merged above
					// note merge already updated dst to skip this branch so just continue
					continue;
				}
				else
				{
					// found dst in src so update dst with the src data for this node
					struct kowhai_tree_t _src = {target_node, ((uint8_t *)src->data + offset)};
					ret = merge_nodes(dst, &_src);
					if (ret != KOW_STATUS_OK && ret != KOW_STATUS_INVALID_MERGE_NODE_TYPE)
						// if the merged failed not because the types failed then properagate 
						// the error (if the types don't match its not an error we just cannot 
						// merge these nodes so we leave dst alone)
						return ret;
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
		// note if this was a branch we skipped the whole thing above so we are only skipping leaf nodes
		ret = kowhai_get_node_size(dst->desc, &size);
		if (ret != KOW_STATUS_OK)
			return ret;
		dst->data = (uint8_t *)dst->data + size;
		dst->desc++;
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
	return merge(_dst, src);
}

