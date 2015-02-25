#version 430
#define MAX_LIGHTS 50
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

uniform mat4 proj;

layout (local_size_x = 32, local_size_y = 32) in;

shared uint pointLightIndex[MAX_LIGHTS];
shared uint pointLightCount = 0;

void main()
{
    vec2 size = vec2(gl_NumWorkGroups.x * 32, gl_NumWorkGroups.y * 32);
    
	ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    vec2 tilePos = vec2(gl_WorkGroupID.xy * gl_WorkGroupSize.xy) / size;
    
    vec2 tileScale = size * (1.0f / float(2*32));
    vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

    vec4 c1 = vec4(-proj[0][0] * tileScale.x, 0.0f, tileBias.x, 0.0f);
    vec4 c2 = vec4(0.0f, -proj[1][1] * tileScale.y, tileBias.y, 0.0f);
    vec4 c4 = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	
    // Derive frustum planes
    vec4 frustumPlanes[6];
    // Sides
    //right
    frustumPlanes[0] = c4 - c1;
    //left
    frustumPlanes[1] = c4 + c1;
    //bottom
    frustumPlanes[2] = c4 - c2;
    //top
    frustumPlanes[3] = c4 + c2;
    // Near/far
    frustumPlanes[4] = vec4(0.0f, 0.0f,  1.0f, -1);
    frustumPlanes[5] = vec4(0.0f, 0.0f, -1.0f,  1);
    
    
    // culling
    
    // something something loop
    uint passCount = 1;
    bool inside = false;
    for (uint passIt = 0; passIt < passCount; ++passIt)
    {
        // something goes here
        
        //
        if(inside)
        {
            uint id = atomicAdd(pointLightCount, 1);
            //pointLightIndex[id] = lightIndex;
        }
    }
        
    
    barrier();
    //imageStore(destTex, pixelPos, vec4(l[0].color.rgb, 0.0f));
    imageStore(destTex, pixelPos, vec4(normalize(vec3(l[2].r, l[2].g, l[2].b)), 0.0f));
}