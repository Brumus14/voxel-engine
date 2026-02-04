#ifndef BLOCK_H
#define BLOCK_H

enum block_type {
    BLOCK_TYPE_EMPTY = 0,
    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_DIRT,
    BLOCK_TYPE_STONE,
    BLOCK_TYPE_COAL,
    BLOCK_TYPE_LOG,
    BLOCK_TYPE_DIAMOND,
    BLOCK_TYPE_LEAF,
};

enum block_face {
    BLOCK_FACE_FRONT = 0,
    BLOCK_FACE_TOP,
    BLOCK_FACE_RIGHT,
    BLOCK_FACE_BOTTOM,
    BLOCK_FACE_LEFT,
    BLOCK_FACE_BACK,
};

// rename to texture indexes?
struct block_texture {
    int face_texture_indices[6]; // front, top, right, bottom, left, back
};

struct block_texture block_type_to_texture(enum block_type type);

struct block {
    enum block_type type;
    // orientation
    // light level
};

#endif
