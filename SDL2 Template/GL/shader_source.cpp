//
//  shader.cpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/29/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "GL/OpenGL.h"
#include "GL/Shader.h"

std::string shader::vLit = R"GLSL(
#version 150

uniform mat4 transform = mat4(1.0);

layout (std140)
uniform
view {
    mat4 camera, projection;
};

in vec3 position, normal;
in vec2 uv;

out mat4 modelview;

out vec3 frag_position, frag_normal;
out vec2 frag_uv;

void main() {
    modelview = camera * transform;
    vec4 eye_position = modelview * vec4(position, 1.0);
    gl_Position = projection * eye_position;
    frag_position = eye_position.xyz;
    frag_normal   = (modelview * vec4(normal, 0.0)).xyz;
    frag_uv = uv;
}
)GLSL";

std::string shader::fLit = R"GLSL(
#version 150

in mat4 modelview;

layout (std140)
uniform
view {
    mat4 camera, projection;
};

in vec3 frag_position, frag_normal;
in vec2 frag_uv;

uniform vec4 tint = vec4(1);

const vec3 light_direction = vec3(0.408248, -0.816497, -0.408248);

out vec4 fragColor;

void main() {
    vec3 normal = normalize(frag_normal);

    float shade = 0.5 * (-dot(normal, normalize(light_direction)) + 1.0);
    fragColor = vec4(tint.rgb * shade, tint.a);
}
)GLSL";
