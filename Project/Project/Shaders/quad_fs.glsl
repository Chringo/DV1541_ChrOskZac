#version 430

in vec2 vertex_texCoord0;

uniform sampler2D diffuse;
uniform sampler2D lightMap;

uniform vec2 screensize;

out vec4 fragment_color;

float gaussianKernel[] = {
0.031827,	0.037541,	0.039665,	0.037541,	0.031827,
0.037541,	0.044281,	0.046787,	0.044281,	0.037541,
0.039665,	0.046787,	0.049434,	0.046787,	0.039665,
0.037541,	0.044281,	0.046787,	0.044281,	0.037541,
0.031827,	0.037541,	0.039665,	0.037541,	0.031827
};

vec4 gaussianBlur(sampler2D tex, vec2 coords)
{
    vec4 add = vec4(0.0);
    int samples = 0;
    for(int x = -2; x < 2; x++)
    {
        for(int y = -2; y < 2; y++)
        {
          add += texture(tex, coords + vec2(x, y)/screensize ) * gaussianKernel[samples];
          samples++;
        }
    }
    
    add *= 1.5;
    
    return add;
}

void main () {
	vec4 diffuse_color = texture(diffuse, vertex_texCoord0);
    vec4 light_color;
    if((vertex_texCoord0.x + vertex_texCoord0.y)/2 < 0.5)
        light_color = gaussianBlur(lightMap, vertex_texCoord0);
    else
        light_color = texture(lightMap, vertex_texCoord0);
        
    fragment_color = diffuse_color * (light_color);
}