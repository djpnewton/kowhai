#ifndef _KOWHAI_UTILS_H_
#define _KOWHAI_UTILS_H_

#include "kowhai.h" 

/**
 * @brief merge nodes that are common to src and dst from src into dst leaving unique nodes unchanged
 * @brief dst, destination tree (this is updated from the source tree)
 * @brief src, source tree
 * @return KOW_STATUS_OK on success otherwise other KOW_STATUS code
 */
int kowhai_merge(struct kowhai_tree_t *dst, struct kowhai_tree_t *src);

#endif

