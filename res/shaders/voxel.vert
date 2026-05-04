#version 460 core

out vec3 position_frag;
out vec2 texture_coordinate_frag;
out vec3 normal_frag;

struct face {
    uint data;
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

vec2 get_tile_texture_coordinate(uint face_tile_index) {
    // TODO: Maybe pass in tile x and y instead of just index
    uint tile_x = face_tile_index % tilemap_tiles_width;
    uint tile_y = face_tile_index / tilemap_tiles_width;

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
    uint face_data = faces[gl_InstanceID].data;
    uint face_x = face_data >> 28;
    uint face_y = (face_data >> 24) & ((1 << 4) - 1);
    uint face_z = (face_data >> 20) & ((1 << 4) - 1);
    uint face_direction = (face_data >> 17) & ((1 << 3) - 1);
    uint face_tile_index = face_data & ((1 << 17) - 1);

    vec3 offset;
    get_position_offset(face_direction, offset);

    vec3 position = vec3(face_x, face_y,face_z) + vec3(offset) + chunk_position;

    gl_Position = projection * view * model * vec4(position, 1.0);

    position_frag = vec3(model * vec4(position, 1.0));
    texture_coordinate_frag = get_tile_texture_coordinate(face_tile_index);
    normal_frag = normals[face_direction];
}
