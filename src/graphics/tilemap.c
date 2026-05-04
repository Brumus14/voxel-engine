#include "tilemap.h"
#include "texture.h"

#include <stdio.h>

void tilemap_init(struct tilemap *tilemap, char *texture_path,
                  enum texture_filter texture_filter, int tile_width,
                  int tile_height, int margin, int spacing) {
    texture_init(&tilemap->texture, texture_filter, TEXTURE_WRAP_REPEAT);
    texture_load(&tilemap->texture, texture_path);

    tilemap->width = tilemap->texture.size.x;
    tilemap->height = tilemap->texture.size.y;

    // remove width and height figure out using texture width and height
    tilemap->tiles_width =
        (tilemap->width - 2 * margin - tile_width) / (tile_width + spacing) + 1;
    tilemap->tiles_height =
        (tilemap->height - 2 * margin - tile_height) / (tile_height + spacing) +
        1;
    tilemap->tile_width = tile_width;
    tilemap->tile_height = tile_height;
    tilemap->margin = margin;
    tilemap->spacing = spacing;
}

void tilemap_destroy(struct tilemap *tilemap) {
    texture_destroy(&tilemap->texture);
}

struct rectangle tilemap_get_tile_rectangle(struct tilemap *tilemap,
                                            int tile_index) {
    struct rectangle rectangle;

    int tile_x = tile_index % tilemap->tiles_width;
    int tile_y = tile_index / tilemap->tiles_width;

    rectangle.x = (tilemap->margin + tile_x * tilemap->tile_width +
                   tile_x * tilemap->spacing) *
                  (1.0 / tilemap->width);
    rectangle.y = (tilemap->margin + tile_y * tilemap->tile_height +
                   tile_y * tilemap->spacing) *
                  (1.0 / tilemap->height);
    rectangle.width = tilemap->tile_width * (1.0 / tilemap->width);
    rectangle.height = tilemap->tile_height * (1.0 / tilemap->height);

    return rectangle;
}

void tilemap_bind(struct tilemap *tilemap) {
    texture_bind(&tilemap->texture);
}
