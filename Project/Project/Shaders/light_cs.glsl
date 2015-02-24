#version 430

uniform float roll;
layout(binding = 0, rgba32f) uniform image2D destTex;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D worldPos;

layout (local_size_x = 32, local_size_y = 32) in;

void main()
{
	 ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	 float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);
	 float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;

	imageStore(destTex, storePos, vec4(1.0-globalCoef*localCoef, 0.0, 0.0, 0.0));
}