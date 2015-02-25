#version 430

uniform float roll;

layout(binding = 0, rgba32f) uniform image2D destTex;

struct Light
{
    uint id;
    float x, y, z;
    float r, g, b;
    //vec3 pos;
    //vec3 color;
    float distance;
};

layout(std430, binding = 1) buffer LightBuffer
{
    Light l[];
};

uniform int nrLights;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D worldPos;

layout (local_size_x = 32, local_size_y = 32) in;

void main()
{
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);
	float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;

	float Sx = gl_NumWorkGroups.x * 32;
	float Sy = gl_NumWorkGroups.y * 32;

	vec2 center = vec2(1.0);
	vec2 z, c;
	int iter = 100;
	float scale = 2.0f;
	vec2 pos = vec2(gl_GlobalInvocationID.x / Sx, gl_GlobalInvocationID.y / Sy);
	
	c.x = 1.3333 * (pos.x - 0.2f) * scale - center.x;
	c.y = (pos.y) * scale - center.y;

	int i;
	z = c;
	for(i=0; i<iter; i++) {
		float x = (z.x * z.x - z.y * z.y) + c.x;
		float y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > 4.0) break;
		z.x = x;
		z.y = y;
	}
    /*if(l.length() == 1)
        imageStore(destTex, storePos, vec4(0.0f, 0.0f, 0.0f, 0.0f));
    if(l.length() == 1)
        imageStore(destTex, storePos, vec4(1.0f, 0.0f, 0.0f, 0.0f));
    else if(l.length() == 2)
        imageStore(destTex, storePos, vec4(0.0f, 1.0f, 0.0f, 0.0f));
    else if(l.length() == 3)
        imageStore(destTex, storePos, vec4(0.0f, 0.0f, 1.0f, 0.0f));
    else
    {
        imageStore(destTex, storePos, normalize(vec4(l.length()/nrLights, 1.0f, 1.0f, 0.0f)));
    }*/
    
    //imageStore(destTex, storePos, vec4(l[0].color.rgb, 0.0f));
    imageStore(destTex, storePos, vec4(normalize(vec3(l[2].r, l[2].g, l[2].b)), 0.0f));
}