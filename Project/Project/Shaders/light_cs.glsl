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
uniform sampler2D shadowMap;

uniform vec2 screensize;

uniform mat4 lightView;

uniform mat4 proj;
uniform mat4 view;

shared uint pointLightIndex[MAX_LIGHTS];
shared Light pointLight[MAX_LIGHTS];
shared uint pointLightCount = 0;

const int sample_count = 16;

vec3 pSphere[16] = vec3[](vec3(0.53812504, 0.18565957, -0.43192),vec3(0.13790712, 0.24864247, 0.44301823),vec3(0.33715037, 0.56794053, -0.005789503),vec3(-0.6999805, -0.04511441, -0.0019965635),vec3(0.06896307, -0.15983082, -0.85477847),vec3(0.056099437, 0.006954967, -0.1843352),vec3(-0.014653638, 0.14027752, 0.0762037),vec3(0.010019933, -0.1924225, -0.034443386),vec3(-0.35775623, -0.5301969, -0.43581226),vec3(-0.3169221, 0.106360726, 0.015860917),vec3(0.010350345, -0.58698344, 0.0046293875),vec3(-0.08972908, -0.49408212, 0.3287904),vec3(0.7119986, -0.0154690035, -0.09183723),vec3(-0.053382345, 0.059675813, -0.5411899),vec3(0.035267662, -0.063188605, 0.54602677),vec3(-0.47761092, 0.2847911, -0.0271716));


layout (local_size_x = WORKGROUP_SIZE, local_size_y = WORKGROUP_SIZE) in;

void main()
{
    vec2 size = screensize;
    
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
    vec3 lightColor = vec3(0.0f);
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
                pointLight[id] = pLight;
                
            }
        }
    }
    barrier();
    
    
    //if(pointLightCount > 0)
    //{
    //    LightColor = vec3(0.25f * pointLightCount);
    //}
    
    // LIGHT CALCULATION
    
    vec4 position = imageLoad(worldPos, pixelPos);
    vec4 normal = imageLoad(normalTex, pixelPos);
    vec3 n = normalize(normal.xyz);
    vec3 v = normalize(vec3(-position.xyz));
    float shinyPower = 1000.0f;
    
    for(uint index = 0; index < pointLightCount; index++)
    {
        //Light pLight = l[pointLightIndex[index]];
        Light pLight = pointLight[index];
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
    		vec3 specularLight = 0 * pLight.color.rgb * pow(max(dot(r, v), 0.0), shinyPower);
            
            lightColor += pLight.color.rgb * attenuation * max(dot(n, s), 0) + specularLight;
            
        }
    }
    
    
    // SSAO
    
    vec2 sampleCoord = (vec2(gl_GlobalInvocationID.xy) / size.xy);
    vec4 vPos = position;
    vec4 wNorm = vec4(n, 1.0f);
    
    float ambientOcclusion = 0;
    
    float distanceThreshold = 1;
    
    for(int i = 0; i < sample_count; ++i)
    {
        vec4 poissonPos = (vPos + vec4(pSphere[i] * 0.25 , 1.0));
        vec4 sampleProj = proj * view * poissonPos;
        sampleProj /= sampleProj.w;
        sampleProj = (sampleProj + 1) / 2;
        
        vec2 sampleTexCoord = sampleProj.xy;
        vec4 samplePos = (texture(worldPosSampler, sampleTexCoord));
        vec4 sampleDir = normalize(samplePos - vPos);
        
        float NdotS = max(dot(wNorm, sampleDir), 0);
        
        float VPdistSP = distance(vPos, samplePos);
        
        float a = 1.0 - smoothstep(distanceThreshold, distanceThreshold*2, VPdistSP);
        float b = NdotS;
        
        ambientOcclusion += (a * b);
    }
    float ao = (ambientOcclusion / sample_count);
    
    ao = (1 - 2*ao);
    
    // SHADOWMAPPING
    
    float shadowFactor = 0.0f;
    
    vec4 shadowCoord = proj * lightView * vec4(position.xyz, 1.0f);
    
    shadowCoord /= shadowCoord.w;
    shadowCoord = (shadowCoord + 1) / 2;
    
    float cosTheta = dot(normalize(wNorm.xyz), vec3(1));
    
    float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0.0, 0.01);
    
    if ( texture( shadowMap, shadowCoord.xy ).x  > (shadowCoord.z - bias))
    {
        shadowFactor = 0.2;
    }
    
    vec3 s = normalize(vec3(vec3(3.0,8.0,-4.0) - position.xyz));
    
    float shadowLight = max(dot(wNorm.xyz, s), 0.0);
    
    vec3 finalColor = vec3(1.0, 1.0, 1.0) * shadowLight * shadowFactor + shadowFactor * lightColor.rgb * (vec3(0.1) + ao) * 2;
    
    imageStore(destTex, pixelPos, vec4(finalColor, 0.0f));
}
