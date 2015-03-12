#version 430
layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main () {

        vec3 ab = (view * model * gl_in[1].gl_Position - view * model * gl_in[0].gl_Position).xyz;
        vec3 ac = (view * model * gl_in[2].gl_Position - view * model * gl_in[0].gl_Position).xyz;
        
        vec3 normal = normalize(cross( ab, ac));
        
		float d = dot( (view * model * -gl_in[0].gl_Position).xyz, normal);

		if(!(d > 0.0f))
		{
			for( int i = 0; i < 3; i++ )
			{
				gl_Position =  projection * view * model * vec4(gl_in[i].gl_Position.xyz, 1.0f);
				EmitVertex();
			}
			EndPrimitive();
		}
}