#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 inVAtan=vec2(0.1591,0.3183);
const float PI=3.14159265359;

vec2 SampleSphericalMap(vec3 V){
	vec2 uv=vec2(atan(V.z,V.x),asin(V.y));
	uv*=inVAtan;
	uv+=0.5;
	return uv;
}

void main()
{    
	vec2 uv=SampleSphericalMap(normalize(localPos));

	vec3 color=texture(equirectangularMap,uv).rgb;

	FragColor=vec4(color,1.0);
}