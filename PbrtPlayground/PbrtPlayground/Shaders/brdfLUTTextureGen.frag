#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
const float PI=3.14159265359;

float RadicalInverse_VdC(uint bits){

	bits=(bits<<16u)|(bits>>16u);
	bits=((bits&0x55555555u)<<1u)|((bits&0xAAAAAAAAu)>>1u);
	bits=((bits&0x33333333u)<<2u)|((bits&0xCCCCCCCCu)>>2u);
	bits=((bits&0x0F0F0F0Fu)<<4u)|((bits&0xF0F0F0F0u)>>4u);
	bits=((bits&0x00FF00FFu)<<8u)|((bits&0xFF00FF00u)>>8u);

	return float(bits)*2.3283064365386963e-10;
}

vec2 Hammersley(uint i,uint N){
	return vec2(float(i)/float(N),RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi,vec3 N,float roughness){
	float a=roughness*roughness;
	float a2=a*a;

	float phi=2.0*PI*Xi.x;
	float cosTheta=sqrt((1-Xi.y)
	/(	1.0+(a2-1.0)*Xi.y)	);
	float sinTheta=sqrt(1-cosTheta*cosTheta);

	//sphere coordinate to tangent coordinates
	vec3 H;
	H.x=cos(phi)*sinTheta;
	H.y=sin(phi)*sinTheta;
	H.z=cosTheta;

	//tangent coordinates to world coordinates
	vec3 up=abs(N.z)<0.999?vec3(0.0,0.0,1.0):vec3(1.0,0.0,0.0);
	vec3 tangent=normalize(cross(up,N));
	vec3 bitangent=cross(N,tangent);
	//wrong place up->N
	vec3 sampleVec=tangent*H.x+bitangent*H.y+N*H.z;

	return normalize(sampleVec);
}


float GeometrySchlickGGX(float NdotV,float roughness){
	float a=roughness;
	float k=(a*a)/2.0;

	float nom=NdotV;
	float denom=NdotV*(1.0-k)+k;

	return nom/denom;
}


float GeometrySmith(float NdotV,float NdotL,float roughness){
	float ggx1=GeometrySchlickGGX(NdotV,roughness);
	float ggx2=GeometrySchlickGGX(NdotL,roughness);
	
	return ggx1*ggx2;
}

vec2 IntergrateBRDF(float NdotV,float roughness){
	float A;
	float B;
	vec3 N=vec3(0,0,1);

	vec3 V;
	V.x=sqrt(1-NdotV*NdotV);
	V.y=0.0;
	V.z=NdotV;
	const uint SAMPLE_COUNT=1024u;
	for(uint i=0u;i<SAMPLE_COUNT;++i){
	
		vec2 Xi=Hammersley(i,SAMPLE_COUNT);
		vec3 H=ImportanceSampleGGX(Xi,N,roughness);
		vec3 L=normalize(2.0*dot(V,H)*H-V);

		float NdotL=max(L.z,0.0);
		float NdotH=max(H.z,0.0);
		float VdotH=max(dot(V,H),0.0);
		
		if(NdotL>0.0){
			float G=GeometrySmith(NdotV,NdotL,roughness);
			float G_Vis=(G*VdotH)/(NdotH*NdotV);
			float Fc=pow((1-VdotH),5.0);
			
			A+=(1.0-Fc)*G_Vis;
			B+=Fc*G_Vis;	
		}	
	}
	A/=float(SAMPLE_COUNT);
	B/=float(SAMPLE_COUNT);
	return vec2(A,B);
}


void main()
{    
	vec2 intergrateBRDF=IntergrateBRDF(TexCoords.x,TexCoords.y);

	FragColor=vec4(intergrateBRDF,0.0,1.0);
}