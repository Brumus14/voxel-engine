#ifndef TILEMAP_H
#define TILEMAP_H

#include "../graphics/texture.h"
#include "../math/rectangle.h"

struct tilemap {
    struct texture texture;
    int width;
    int height;
    int tile_width;
    int tile_height;
    int margin;
    int spacing;
};

void tilemap_init(struct tilemap *tilemap, char *texture_path,
                  enum texture_filter texture_filter, int tile_width,
                  int tile_height, int margin, int spacing);
struct rectangle tilemap_get_tile_rectangle(struct tilemap *tilemap,
                                            int tile_index);
void tilemap_bind(struct tilemap *tilemap);

#endif
