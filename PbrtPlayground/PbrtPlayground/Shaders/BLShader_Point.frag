#version 330 core
layout(location=0)out vec4 FragColor;
layout(location=1)out vec4 BrightColor;

in vec2 TexCoords;
//texture

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gTangentViewDir;
uniform sampler2D gTangentLightDir;

uniform samplerCube shadowDepthMap;

//shadow and lighting config
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 viewPos;
uniform float far_plane;


vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos){

/* normal PCF
	 vec3 fragToLight = fragPos - LightPos;
	 float currentDepth = length(fragToLight);
	 float closestDepth = texture(shadowDepthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
	 closestDepth *= far_plane;
     //PCF
     float shadow = 0.0;
     float bias = 0.05; 
     float samples =4.0;
     float offset = 0.01;
     for(float x = -offset; x < offset; x += offset / (samples * 0.5))
     {
         for(float y = -offset; y < offset; y += offset / (samples * 0.5))
         {
             for(float z = -offset; z < offset; z += offset / (samples * 0.5))
             {
                 float closestDepth = texture(shadowDepthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                 closestDepth *= far_plane;   // Undo mapping [0;1]
                 if(currentDepth - bias > closestDepth)
                     shadow += 1.0;
             }
         }
     }
	shadow /= (samples * samples * samples);

*/





	vec3  fragToLight=fragPos-LightPos;
	float currentDepth=length(fragToLight);

	float shadow=0.0;
	float bias =0.1;
	int	  samples=20;

	float ViewDistance=length(viewPos-fragPos);
	//Adjust shadow quality  The smaller the DiskRadius value,the better the shadow quality. 
	float diskRadius=(1.0 + (ViewDistance / far_plane)) / 100.0f;

	for(int i=0;i<samples;++i){
		float closestDepth=texture(shadowDepthMap,fragToLight+sampleOffsetDirections[i]*diskRadius).r;
		closestDepth*=far_plane;

		if(currentDepth-bias>closestDepth)
			shadow+=1.0;
	}
	shadow/=float(samples);

	return shadow;
}


void main(){      

	vec3 fragPos=texture(gPosition,TexCoords).rgb;
	
	vec3 viewDir=texture(gTangentViewDir,TexCoords).rgb;

    vec3 color = texture(gAlbedoSpec,TexCoords).rgb;
	float specularColor=texture(gAlbedoSpec,TexCoords).a;
	
	vec3 normal=texture(gNormal,TexCoords).rgb;


   
    // ambient
    vec3 ambient =0.1f * color;
    // diffuse
    vec3 lightDir = texture(gTangentLightDir,TexCoords).rgb;
    float diff = max(dot(normal,lightDir ), 0.0);
    vec3 diffuse = diff * color;
    // specular
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec =  pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = LightColor*spec*specularColor;    


	float distance= length(lightDir);
	float constant=1.0f;
	float linear=0.35f;
	float quadratic=0.44f;
	//Without Gamma Correction
    //float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));  
	//After Gamma Correction
    float attenuation = 1.0 / (quadratic * distance);  

	




    // calculate shadow
	
	
    float shadow = ShadowCalculation(fragPos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color*attenuation;    
    
    FragColor = vec4(lighting, 1.0f);

	float brightness=dot(FragColor.rgb,vec3(0.2126,0.7152,0.0722));
	if(brightness>1.0f)
		BrightColor=vec4(FragColor.rgb,1.0f);
	else
		BrightColor=vec4(0.0f,0.0f,0.0f,1.0f);


	//float closestDepth=texture(shadowDepthMap,fs_in.FragPos-LightPos).r;
	

	//FragColor = vec4(vec3(closestDepth), 1.0);
}