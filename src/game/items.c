#include "items.h"

enum block_type item_type_to_block_type(enum item_type type) {
    switch (type) {
    case ITEM_TYPE_GRASS_BLOCK:
        return BLOCK_TYPE_GRASS;
    case ITEM_TYPE_DIRT_BLOCK:
        return BLOCK_TYPE_DIRT;
    case ITEM_TYPE_STONE_BLOCK:
        return BLOCK_TYPE_STONE;
    case ITEM_TYPE_COAL_BLOCK:
        return BLOCK_TYPE_COAL;
    case ITEM_TYPE_LOG_BLOCK:
        return BLOCK_TYPE_LOG;
    case ITEM_TYPE_DIAMOND_BLOCK:
        return BLOCK_TYPE_DIAMOND;
    default:
        return -1;
    }
}

enum item_type block_type_to_item_type(enum block_type type) {
    switch (type) {
    case BLOCK_TYPE_GRASS:
        return ITEM_TYPE_GRASS_BLOCK;
    case BLOCK_TYPE_DIRT:
        return ITEM_TYPE_DIRT_BLOCK;
    case BLOCK_TYPE_STONE:
        return ITEM_TYPE_STONE_BLOCK;
    case BLOCK_TYPE_COAL:
        return ITEM_TYPE_COAL_BLOCK;
    case BLOCK_TYPE_LOG:
        return ITEM_TYPE_LOG_BLOCK;
    case BLOCK_TYPE_DIAMOND:
        return ITEM_TYPE_DIAMOND_BLOCK;
    default:
        return -1;
    }
}

int item_type_to_texture_index(enum item_type type) {
    switch (type) {
    case ITEM_TYPE_GRASS_BLOCK:
        return 0;
    case ITEM_TYPE_DIRT_BLOCK:
        return 1;
    case ITEM_TYPE_STONE_BLOCK:
        return 2;
    case ITEM_TYPE_COAL_BLOCK:
        return 3;
    case ITEM_TYPE_LOG_BLOCK:
        return 4;
    case ITEM_TYPE_DIAMOND_BLOCK:
        return 5;
    default:
        return -1;
    }
}
