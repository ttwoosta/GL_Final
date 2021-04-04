#version 420

uniform mat4 transform = mat4(1.0);

layout (binding = 1, std140)
uniform
view {
    mat4 camera, projection;
};

layout (location = 0) 
in vec3 position;
layout (location = 1)
in vec3 normal;
layout (location = 2) 
in vec2 uv;

out vec3 vertex_position, vertex_normal;
out vec2 vertex_uv;

void main() {
    mat4 modelview = camera * transform;
    vec4 eye_position = modelview * vec4(position, 1.0);
    gl_Position = projection * eye_position;
    vertex_position = vec3(eye_position);
    vertex_normal = vec3(modelview * vec4(normal, 0));
    vertex_uv = uv;
}