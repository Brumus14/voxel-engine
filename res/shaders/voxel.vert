#version 460 core

out vec3 position_frag;
out vec2 texture_coordinate_frag;
out vec3 normal_frag;

struct face {
    uint x;
    uint y;
    uint z;
    uint direction;
};

layout (binding = 0, std430) readonly buffer ssbo {
    face faces[];
};

uniform vec3 chunk_position;

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

const vec2 texture_coordinates[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 0.0)
);

vec3 get_position_offset(int direction) {

}

void main() {
    uint face_index = gl_InstanceID;
    uint vertex_index = gl_VertexID;

    vec3 position = vec3(faces[face_index].x, faces[face_index].y,faces[face_index].z) + vec3(offsets[vertex_index],0) + chunk_position;

    gl_Position = projection * view * model * vec4(pos, 1.0);

    position_frag = vec3(model * vec4(pos, 1.0));
    texture_coordinate_frag = texture_coordinates[vertex_index];
    normal_frag = vec3(0,0,1);
}
