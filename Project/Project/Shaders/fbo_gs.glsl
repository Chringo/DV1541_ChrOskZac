#version 430
layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 colorG[];
in vec3 worldPosG[];
in vec3 vn[];

out vec3 color;
out vec3 normal;
out vec3 worldPos;

void main () {
		float d = dot( (view * model * -gl_in[0].gl_Position).xyz, normal);

		if(d > 0.0f)
		{
			for( int i = 0; i < 3; i++ )
			{
				gl_Position =  projection * view * model * gl_in[i].gl_Position;
				color = colorG[i];
				normal = (model * vec4(vn[i], 0.0f)).xyz;
				worldPos = (model * vec4(worldPosG[i], 1.0f)).xyz;
				EmitVertex();
			}
			EndPrimitive();
		}
}