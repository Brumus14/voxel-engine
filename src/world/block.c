#include "block.h"

struct block_texture block_type_to_texture(enum block_type type) {
    // front, top, right, bottom, left, back
    switch (type) {
    case BLOCK_TYPE_EMPTY:
        // Use enums instead?
        return (struct block_texture){0, 0, 0, 0, 0, 0};
    case BLOCK_TYPE_GRASS:
        return (struct block_texture){1, 0, 1, 2, 1, 1};
    case BLOCK_TYPE_DIRT:
        return (struct block_texture){3, 2, 3, 2, 3, 3};
    case BLOCK_TYPE_STONE:
        return (struct block_texture){4, 4, 4, 4, 4, 4};
    case BLOCK_TYPE_COAL:
        return (struct block_texture){5, 5, 5, 5, 5, 5};
    case BLOCK_TYPE_LOG:
        return (struct block_texture){6, 7, 6, 7, 6, 6};
    case BLOCK_TYPE_DIAMOND:
        return (struct block_texture){8, 8, 8, 8, 8, 8};
    case BLOCK_TYPE_LEAF:
        return (struct block_texture){9, 9, 9, 9, 9, 9};
    }
}
