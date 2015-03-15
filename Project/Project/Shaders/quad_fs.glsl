#version 430

in vec2 vertex_texCoord0;

uniform sampler2D diffuse;
uniform sampler2D lightMap;
uniform sampler2D shadowMap;

out vec4 fragment_color;

void main () {
	vec4 diffuse_color = texture(diffuse, vertex_texCoord0);
	vec4 light_color = texture(lightMap, vertex_texCoord0);
    vec4 depth_color = texture(shadowMap, vertex_texCoord0);
	fragment_color = diffuse_color * (light_color);
    
    float d = depth_color.x;
    
    d = 1 - ( 1- d) * 100;
    
    //fragment_color = vec4(d);
}