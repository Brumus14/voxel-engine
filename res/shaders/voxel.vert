#version 460 core

out vec3 position_frag;
out vec2 texture_coordinate_frag;
out vec3 normal_frag;

struct face {
    uint x;
    uint y;
    uint z;
    uint direction;
    uint tile_index;
};

layout (binding = 0, std430) readonly buffer ssbo {
    face faces[];
};

uniform vec3 chunk_position;
uniform uint tilemap_width;
uniform uint tilemap_height;
uniform uint tilemap_tiles_width;
uniform uint tilemap_tiles_height;
uniform uint tilemap_tile_width;
uniform uint tilemap_tile_height;
uniform uint tilemap_margin;
uniform uint tilemap_spacing;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec2 position_offsets[6] =vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

const vec3 normals[6] = vec3[](
    vec3(-1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, -1.0),
    vec3(0.0, 0.0, 1.0)
);

const vec2 texture_coordinates[6] = vec2[](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

vec2 get_tile_texture_coordinate() {
    // TODO: Maybe pass in tile x and y instead of just index
    uint tile_x = faces[gl_InstanceID].tile_index % tilemap_tiles_width;
    uint tile_y = faces[gl_InstanceID].tile_index / tilemap_tiles_width;

    vec2 tile_origin = vec2(tilemap_margin+(tilemap_tile_width + tilemap_spacing) * tile_x, tilemap_margin+(tilemap_tile_height + tilemap_spacing) * tile_y);
    return (tile_origin + texture_coordinates[gl_VertexID] * vec2(tilemap_tile_width, tilemap_tile_height)) / vec2(tilemap_width, tilemap_height);
}

void get_position_offset(uint direction, out vec3 offset) {
        switch (direction) {
            case 0:
                offset = vec3(0, position_offsets[gl_VertexID].y, position_offsets[gl_VertexID].x);
                return;
            case 1:
                offset = vec3(1, position_offsets[gl_VertexID].y, 1-position_offsets[gl_VertexID].x);
                return;
            case 2:
                offset = vec3(position_offsets[gl_VertexID].x, 0, position_offsets[gl_VertexID].y);
                return;
            case 3:
                offset = vec3(position_offsets[gl_VertexID].x, 1, 1-position_offsets[gl_VertexID].y);
                return;
            case 4:
                offset = vec3(1-position_offsets[gl_VertexID].x, position_offsets[gl_VertexID].y, 0);
                return;
            case 5:
                offset = vec3(position_offsets[gl_VertexID].x, position_offsets[gl_VertexID].y, 1);
                return;
    }
}

void main() {
    vec3 offset;
    get_position_offset(faces[gl_InstanceID].direction, offset);

    vec3 position = vec3(faces[gl_InstanceID].x, faces[gl_InstanceID].y,faces[gl_InstanceID].z) + vec3(offset) + chunk_position;

    gl_Position = projection * view * model * vec4(position, 1.0);

    position_frag = vec3(model * vec4(position, 1.0));
    texture_coordinate_frag = get_tile_texture_coordinate();
    normal_frag = normals[faces[gl_InstanceID].direction];
}
