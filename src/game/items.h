#ifndef ITEMS_H
#define ITEMS_H

#include "../world/block.h"

enum item_type {
    ITEM_TYPE_EMPTY = 0,
    ITEM_TYPE_GRASS_BLOCK,
    ITEM_TYPE_DIRT_BLOCK,
    ITEM_TYPE_STONE_BLOCK,
    ITEM_TYPE_COAL_BLOCK,
    ITEM_TYPE_LOG_BLOCK,
    ITEM_TYPE_DIAMOND_BLOCK,
};

enum block_type item_type_to_block_type(enum item_type type);
enum item_type block_type_to_item_type(enum block_type type);
int item_type_to_texture_index(enum item_type type);

#endif
