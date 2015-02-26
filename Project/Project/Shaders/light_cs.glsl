#version 430
#define WORKGROUP_SIZE 32
#define PIXEL_PER_THREAD 4
#define MAX_LIGHTS 30

struct Light
{
    vec4 pos;
    vec4 color;
};

layout(binding = 0, rgba32f) uniform image2D writeonly destTex;
layout(std430, binding = 1) buffer LightBuffer
{
    Light l[];
};
layout(binding = 2, rgba32f)uniform image2D readonly diffuseTex;
layout(binding = 3, rgba32f)uniform image2D readonly normalTex;
layout(binding = 4, rgba32f)uniform image2D readonly worldPos;

uniform mat4 proj;
uniform mat4 view;

shared uint pointLightIndex[MAX_LIGHTS];
shared uint pointLightCount = 0;

layout (local_size_x = WORKGROUP_SIZE, local_size_y = WORKGROUP_SIZE) in;

void main()
{
    vec2 size = vec2(gl_NumWorkGroups.x * WORKGROUP_SIZE, gl_NumWorkGroups.y * WORKGROUP_SIZE);
    
	ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    vec2 tilePos = vec2(gl_WorkGroupID.xy * gl_WorkGroupSize.xy) / size;
    
    vec2 tileScale = size * (1.0f / float(2*WORKGROUP_SIZE));
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
    frustumPlanes[4] = vec4(0.0f, 0.0f,  1.0f, -0.1f);
    frustumPlanes[5] = vec4(0.0f, 0.0f, -1.0f,  100.0f);
    
    for(int i = 0; i < 4; i++)
    {
        frustumPlanes[i] *= 1.0f / length(frustumPlanes[i].xyz);
    }
    
    // culling
    
    // something something loop
    // final color;
    vec3 color = vec3(0.0f);
    barrier();
    
    uint threadCount = WORKGROUP_SIZE * WORKGROUP_SIZE;
    uint passCount = (l.length() + threadCount - 1) /threadCount;
    Light pLight;
    vec4 pos;
    float rad, dist;
    uint id;
    bool inFrustum;
    for (uint passIt = 0; passIt < passCount; ++passIt)
    {
        uint lightIndex =  passIt * threadCount + gl_LocalInvocationIndex;
        lightIndex = min(lightIndex, l.length());
        
        pLight = l[lightIndex];
        pos = view * vec4(pLight.pos.xyz, 1.0f);
        rad = pLight.color.w;

        if (pointLightCount < MAX_LIGHTS)
        {
            inFrustum = true;
            for (uint i = 3; i >= 0 && inFrustum; i--)
            {
                dist = dot(frustumPlanes[i], pos);
                inFrustum = (-rad <= dist);
            }

            if (inFrustum)
            {
                id = atomicAdd(pointLightCount, 1);
                pointLightIndex[id] = lightIndex;
                
            }
        }
    }    
    barrier();
    
    
    //if(pointLightCount > 0)
    //{
    //    color = vec3(0.25f * pointLightCount);
    //}
    
    for(uint index = 0; index < pointLightCount; index++)
    {
        Light pLight = l[pointLightIndex[index]];
        vec4 position = imageLoad(worldPos, pixelPos);
        vec4 normal = imageLoad(normalTex, pixelPos);
        float dist = distance(position.xyz, pLight.pos.xyz);
        //if(dist < pLight.color.w)
        //{
            
            float d = pLight.color.w;
            float attenuation;
            if(dist != 0)
                attenuation = 1 - clamp((dist / d), 0, 1);
            
            vec3 n = normalize(normal.xyz);
            vec3 s = normalize(vec3(pLight.pos.xyz - position.xyz));
            vec3 v = normalize(vec3(-position.xyz));
            float shinyPower = 2000.0f;
    
            vec3 r = reflect(-s, n);
    		vec3 specularLight = pLight.color.rgb * pow(max(dot(r, v), 0.0), shinyPower);
            
            color += pLight.color.rgb * attenuation * max(dot(n, s), 0) + specularLight;
            
        //}
    }

    imageStore(destTex, pixelPos, vec4(color.rgb, 0.0f));
    //imageStore(destTex, pixelPos, vec4(normalize(vec3(l[2].r, l[2].g, l[2].b)), 0.0f));

}
