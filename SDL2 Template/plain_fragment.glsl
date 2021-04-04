#version 420

in vec3 vertex_position, vertex_normal;
in vec2 vertex_uv;

layout (std140, binding = 1)
uniform
view {
    mat4 camera, projection;
};

uniform	vec4 tint = vec4(1);
uniform sampler2D surface;

struct source {
	vec3 position;
	int kind;
	vec3 angle;
	float intensity;
	vec3 color;
};

uniform vec3 ambient = vec3(0);
layout(std140, binding = 2)
uniform lighting {
	source light[4];
};

out vec4 fragColor;

vec3 screen (vec3 a, vec3 b) {
	return mix(a, vec3(1.0), b);
}

vec3 light_received(source l, vec3 position, vec3 normal)
{
	vec3 direction = vec3(0);
	float distance = 0;
	switch (l.kind) {
	case 0:
		direction = vec3(0);
		break;
	case 1:
		direction = -normalize(vec3(camera * vec4(l.angle, 0)));
		break;
	case 2:
		vec3 offset = position - vec3(camera * vec4(l.position, 1));
		direction = normalize(offset);
		distance = length(offset);
		break;
	}
	float angle = -dot(direction, normal);
//	return vec3(angle);
	if (l.intensity > 0) { distance /= l.intensity; }
	distance += 1;
	angle /= distance * distance;
	if (angle > 0) {
		return l.color * angle;
	} else {
		direction = vec3(0);
		return vec3(0);
	}
}

void main() {
    vec3 normal = normalize(vertex_normal);
	vec4 albedo = tint * texture(surface, vertex_uv);

	vec3 intensity = ambient; 
	for (int i = 0; i < 4; ++i) {
		vec3 l = light_received(light[i], vertex_position, normal);
		intensity = screen(intensity, l);
	} 

	vec3 diffuse = albedo.rgb * intensity; 
    fragColor = vec4(diffuse, albedo.a);
}