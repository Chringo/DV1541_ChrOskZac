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

uniform sampler2D worldPosSampler;
uniform sampler2D normalSampler;

uniform mat4 proj;
uniform mat4 view;

shared uint pointLightIndex[MAX_LIGHTS];
//shared Light pointLight[MAX_LIGHTS];
shared uint pointLightCount = 0;

const int sample_count = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );

layout (local_size_x = WORKGROUP_SIZE, local_size_y = WORKGROUP_SIZE) in;

void main()
{
    vec2 size = vec2(gl_NumWorkGroups.x * WORKGROUP_SIZE, gl_NumWorkGroups.y * WORKGROUP_SIZE);
    
	ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    
    vec2 tileScale = size * (1.0f / float(2*WORKGROUP_SIZE));
    vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

    vec4 c1 = vec4(-proj[0][0] * tileScale.x, 0.0f, tileBias.x, 0.0f);
    vec4 c2 = vec4(0.0f, -proj[1][1] * tileScale.y, tileBias.y, 0.0f);
    vec4 c4 = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	
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
        if( lightIndex > l.length())
            break;
        //lightIndex = min(lightIndex, l.length());
        
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
                //pointLight[id] = pLight;
                
            }
        }
    }
    barrier();
    
    
    //if(pointLightCount > 0)
    //{
    //    color = vec3(0.25f * pointLightCount);
    //}
    
    
    vec4 position = imageLoad(worldPos, pixelPos);
    vec4 normal = imageLoad(normalTex, pixelPos);
    vec3 n = normalize(normal.xyz);
    vec3 v = normalize(vec3(-position.xyz));
    float shinyPower = 100.0f;
    
    for(uint index = 0; index < pointLightCount; index++)
    {
        Light pLight = l[pointLightIndex[index]];
        //Light pLight = pointLight[index];
        float dist = distance(position.xyz, pLight.pos.xyz);
        //if(dist < pLight.color.w)
        {
            
            float d = pLight.color.w;
            float attenuation;
            if(dist != 0)
                attenuation = 1- clamp((dist / d), 0, 1);
                attenuation = max(attenuation, 0);
            
            vec3 s = normalize(vec3(pLight.pos.xyz - position.xyz));
    
            vec3 r = reflect(-s, n);
    		vec3 specularLight = vec3(0);pLight.color.rgb * pow(max(dot(r, v), 0.0), shinyPower);
            
            color += pLight.color.rgb * attenuation * max(dot(n, s), 0) + specularLight;
            
        }
    }
    
    vec2 sampleCoord = (vec2(gl_GlobalInvocationID.xy) / size);
    vec4 vPos = (texture(worldPosSampler, sampleCoord));
    vec4 wNorm = normalize((texture(normalSampler, sampleCoord)));
    
    float ambientOcclusion = 0;
    
    float distanceThreshold = 500;
    
    for(int i = 0; i < sample_count; ++i)
    {
        //vec4 poissonPos = proj * view * (vPos + vec4(poisson16[i], 0, 0));
        //poissonPos /= poissonPos.w;
        
        vec2 sampleTexCoord = sampleCoord + (poisson16[i] * 0.01);
        vec4 samplePos = (texture(worldPosSampler, sampleTexCoord));
        vec4 sampleDir = normalize(samplePos - vPos);
        
        float NdotS = max(dot(wNorm, sampleDir), 0);
        
        float VPdistSP = distance(vPos, samplePos);
        
        float a = 1.0 - smoothstep(0, 4, VPdistSP);
        float b = NdotS;
        
        ambientOcclusion += (a * b);
    }
    
    //color = vec3(1 - 2*(ambientOcclusion / sample_count), 0, 0 );
    
    imageStore(destTex, pixelPos, vec4(color.rgb, 0.0f));
    //imageStore(destTex, pixelPos, vec4(normalize(vec3(l[2].r, l[2].g, l[2].b)), 0.0f));

}
