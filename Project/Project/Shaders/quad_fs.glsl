#version 430

in vec2 vertex_texCoord0;

uniform sampler2D diffuse;
uniform sampler2D lightMap;
uniform sampler2D shadowMap;

uniform vec2 screensize;

out vec4 fragment_color;

// blur function
vec4 blur(sampler2D tex, vec2 coords)
{
    vec4 add = vec4(0.0);
    int samples = 0;
    for(int x = -4; x < 4; x++)
    {
        for(int y = -4; y < 4; y++)
        {
          add += texture(tex, coords + vec2(x, y)/screensize );
          samples++;
        }
    }
    
    add /= samples;
    
    return add;
}



void main () {
	vec4 diffuse_color = texture(diffuse, vertex_texCoord0);
	vec4 light_color = blur(lightMap, vertex_texCoord0);
    vec4 depth_color = texture(shadowMap, vertex_texCoord0);
    fragment_color = diffuse_color * (light_color);
    
    //fragment_color = light_color;
}