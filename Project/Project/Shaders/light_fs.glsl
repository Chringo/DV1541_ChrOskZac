#version 430

in vec2 vertex_texCoord0;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D worldPos;

layout(location = 0) out vec3 lightBuffer;

vec3 point = vec3(0.0f, 5.0f, 0.0f);
float intens = 1.0f;
vec3 Kd = vec3(1.0f, 1.0f, 1.0f);

void main () {
	vec4 diffuse_color = texture(diffuse, vertex_texCoord0);
	vec4 normal_color = texture(normal, vertex_texCoord0);
	vec4 world_color = texture(worldPos, vertex_texCoord0);
	
	vec3 n = normalize(normal_color.xyz);
	vec3 s = normalize(vec3(point - world_color.xyz));
	
	lightBuffer = Kd * vec3(max(dot(s, n), 0.0)) * intens;
}