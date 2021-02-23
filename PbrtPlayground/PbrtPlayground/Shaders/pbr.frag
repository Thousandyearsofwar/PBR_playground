#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

//material parameter
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D	brdfLUTTexture;



uniform vec3 lightPosition[4];
uniform vec3 lightColors[4];

uniform vec3 CamPos;

const float PI=3.14159265359;


float DistributionGGX(vec3 N,vec3 H,float roughness){
	float a=roughness*roughness;
	float a2=a*a;
	float NdotH=max(dot(N,H),0.0f);
	float NdotH2=NdotH*NdotH;

	float nom=a2;
	float denom=(NdotH2*(a2-1.0f)+1.0f);
	denom=PI*denom*denom;

	return nom/denom;
}
float GeometrySchlickGGX(float NdotV,float roughness){
	float r= (roughness+1.0f);
	float k=(r*r)/8.0f;

	float nom=NdotV;
	float denom=NdotV*(1.0f-k)+k;
	
	return nom/denom;
}
float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness){
	float NdotV=max(dot(N,V),0.0f);
	float NdotL=max(dot(N,L),0.0f);

	float ggx2=GeometrySchlickGGX(NdotV,roughness);
	float ggx1=GeometrySchlickGGX(NdotL,roughness);

	return ggx1*ggx2;
}

vec3 FresnelSchlick(float cosTheta,vec3 F0){
	return F0+(1.0f-F0)*pow(1.0-cosTheta,5.0);
}

void main()
{    
    vec3 N=normalize(Normal);
	vec3 V=normalize(CamPos-WorldPos);
	vec3 R=reflect(-V,N);

	float NdotV=max(dot(N,V),0.0f);

	vec3 F0=vec3(0.04f);
	F0=mix(F0,albedo,metallic);

	vec3  F=FresnelSchlick(NdotV,F0);
	vec3 kS=F;
	vec3 kD=vec3(1.0f)-kS;
	kD*=1.0-metallic;

	vec3 Lo=vec3(0.0f);
	for(int i=0;i<4;++i){
		vec3 L=normalize(lightPosition[i]-WorldPos);
		vec3 H=normalize(V+L);
		float VdotH=max(dot(V,H),0.0f);
		float NdotL=max(dot(N,L),0.0f);

		float dis=length(lightPosition[i]-WorldPos);
		float attenuation= 1.0/(dis*dis);
		vec3 radiance=lightColors[i]*attenuation;

		float D=DistributionGGX(N,H,roughness);
		float G=GeometrySmith(N,V,L,roughness);

		vec3 nominator=D*G*F;
		float denominator=(4*NdotV*NdotL)+0.001;
		vec3 specular=nominator/denominator;

		Lo+=(kD*albedo/PI+specular)*radiance*NdotL;
	}

	//diffuse
	vec3 irradiance=texture(irradianceMap,N).rgb;
	vec3 diffuse=irradiance*albedo;

	//specular
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor=textureLod(prefilterMap,R,roughness*MAX_REFLECTION_LOD).rgb;
	vec3 envBRDF=texture(brdfLUTTexture,vec2(NdotV,roughness)).rgb;
	vec3 specular=prefilteredColor*(F*envBRDF.x+envBRDF.y);


	
	vec3 ambient=(kD*diffuse+specular)*ao;

	vec3 color=ambient+Lo;

	//Tone correct
	color=color/(color+vec3(1.0f));
	color=pow(color,vec3(1.0f/2.2f));
	FragColor=vec4(color,1.0f);

}

