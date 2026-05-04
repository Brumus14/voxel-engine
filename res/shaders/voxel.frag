#version 460 core

in vec3 position_frag;
in vec2 texture_coordinate_frag;
in vec3 normal_frag;

out vec4 frag_color_out;

uniform vec3 camera_position;
uniform sampler2D texture_sampler;

void main() {
    vec4 texture_colour = texture(texture_sampler, texture_coordinate_frag);

    // TODO: refactor this
    vec3 norm = normalize(normal_frag);
    float distance = distance(camera_position,position_frag);

    vec3 light_direction = normalize(camera_position - position_frag);
    float diff = max(dot(norm, light_direction), 0.0) * 0.5 + 0.5;
    vec3 diffuse = diff * vec3(1,1,1);

    vec3 result = diffuse * texture_colour.rgb; // Disco mode: * sin(camera_position)
    vec3 sky = vec3(0.53, 0.81, 0.92);

    result = mix(result, sky, smoothstep(100, 120, distance));

    frag_color_out = vec4(result, texture_colour.a);

    // Black and white
    // float value = length(result);
    // if (mod(int(gl_FragCoord.x / 3), 2) == 0 && mod(int(gl_FragCoord.y / 3), 2) == 0) {
    //     value *= 0.7;
    // }
    // frag_color_out = vec4(vec3(value), 1.0);
}
