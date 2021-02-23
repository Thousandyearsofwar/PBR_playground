#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform float exposure;

void main(){
	const float gamma =2.2f;
	vec3 hdrColor=texture(scene,TexCoords).rgb;

	vec3 bloomColor=texture(bloomBlur,TexCoords).rgb;
	
	//if(bloom)
		hdrColor+=bloomColor;

	//vec3 mapped=hdrColor/(hdrColor+vec3(1.0f));
	vec3 mapped=vec3(1.0f)-exp(-hdrColor*exposure);

	mapped=pow(mapped.rgb,vec3(1.0f/gamma));
	
	FragColor=vec4(mapped,1.0f);
}