#version 330 core
layout(location=0)out vec4 FragColor;

in vec2 TexCoords;
//texture

uniform sampler2D debugTexture;



void main(){      

	vec3 color=texture(debugTexture,TexCoords).rgb;
	FragColor=vec4(color,1.0);
}