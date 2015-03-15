#version 430
in vec3 color;
in vec2 textureNormal;
in vec3 normal;
in vec3 worldPos;

uniform sampler2D objTexture;

//out vec4 fragment_color;

layout (location = 0) out vec3 diffuseOut;
layout (location = 1) out vec3 normalOut; 
layout (location = 2) out vec3 worldOut; 

void main ()
{
	vec4 textureColor = texture(objTexture, textureNormal); 
	diffuseOut = textureColor.rgb * color;
	normalOut = normalize(normal);
	worldOut = worldPos;
}