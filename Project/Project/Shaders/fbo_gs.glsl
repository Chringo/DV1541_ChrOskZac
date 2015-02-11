#version 430
layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 colorG[];
in vec3 worldPosG[];

out vec3 color;
out vec3 normal;
out vec3 worldPos;

void main () {

	//normal = (model * vec4(normalize( cross( vec3(gl_in[1].gl_Position - gl_in[0].gl_Position ), vec3(vec3(gl_in[2].gl_Position -gl_in[0].gl_Position) ) ) ),0.0f )).xyz;

	normal = normalize (cross( vec3(gl_in[1].gl_Position - gl_in[0].gl_Position ), vec3( vec3( gl_in[2].gl_Position - gl_in[0].gl_Position ) ) ) );

	normal = (model * vec4(normal, 1.0f)).xyz;

	for( int i = 0; i < 3; i++ )
	{
		gl_Position =  projection * view * model * gl_in[i].gl_Position;
		color = colorG[i];
		worldPos = (model * vec4(worldPosG[i], 1.0f)).xyz;
		EmitVertex();
	}
	EndPrimitive();
}