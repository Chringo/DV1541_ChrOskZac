#version 430
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_normal;
layout(location = 2) in vec3 vertex_normal;

out vec3 colorG;
out vec3 worldPosG;
out vec2 uv;
out vec3 vn;

void main () {

	colorG = vec3 (1, 1, 1);
	
	//worldPosG = (model * vec4(vertex_position, 1.0)).xyz;
	//gl_Position = projection * view * model * vec4 (vertex_position, 1.0);
	gl_Position = vec4 (vertex_position, 1.0);
	worldPosG = vertex_position;
	
	uv = texture_normal;
	vn = vertex_normal;
}