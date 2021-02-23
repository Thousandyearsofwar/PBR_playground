#version 330 core
layout(location=0)out vec3 gPosition;
layout(location=1)out vec3 gNormal;
layout(location=2)out vec4 gAlbedoSpec;
layout(location=3)out vec3 gTangentViewDir;
layout(location=4)out vec3 gTangentLightDir;

in VS_OUT {
	vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	
	vec3 LightPos;
	vec3 ViewPos;
} fs_in;
//texture
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;



void main(){             
	gPosition=fs_in.FragPos;
	gNormal=texture(texture_normal1,fs_in.TexCoords).rgb;
	gNormal=normalize(gNormal*2.0f-1.0f);
	gAlbedoSpec.rgb= texture(texture_diffuse1,fs_in.TexCoords).rgb;
	gAlbedoSpec.a=texture(texture_specular1,fs_in.TexCoords).r;
	gTangentLightDir=normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	gTangentViewDir=normalize(fs_in.TangentViewPos-fs_in.TangentFragPos);

}