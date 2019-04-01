#version 420

//This shader puts the scene together reading from the GBuffer

uniform mat4 ViewToShadowMap;

uniform vec3 LightDirection;
//Colour
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;
//Scalars
uniform float LightSpecularExponent;


//You can create multiple of these to create multiple slots
uniform sampler2D uScene;
uniform sampler2D uShadowMap;
uniform sampler2D uNormalMap;
uniform sampler2D uPositionMap;
uniform sampler2D uEdgeMap;
uniform sampler2D uStepTexture;
uniform sampler2D ssao;

//Spotlight Uniforms
uniform vec3 uSceneAmbient = vec3(0.0, 1.0, 0.0);
uniform vec3 uLightPosition = vec3(3.0, 0.0, 0.0);
uniform vec3 uLightColor = vec3(0.0, 1.0, 0.0);
uniform float uLightSpecularExponent = 16.0;

uniform bool uToonActive;
uniform bool uAmbient = false;

in vec2 texcoord;
in vec3 norm;
in vec3 pos;

out vec4 outColor;

/*void SpotLight()
{
	vec4 tempColor;
	tempColor.rgb = uSceneAmbient;
	tempColor.a = 0.5f;

	// Fix length after rasterizer interpolates
	vec3 normal = normalize(norm);

	vec3 lightVec = uLightPosition - pos;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;

	float NdotL = dot(normal, lightDir);

	// If the normal is facing the light
	if (NdotL > 0.0)
	{
		// Normalized vector pointing towards the camera
		vec3 eye = normalize(-pos);
		
		// Calculate attenuation (falloff)
		// Add a small number to avoid divide by zero.
		float attenuation = 1.0 / (0.5f + dist * dist * 0.1);

		// Calculate the diffuse contribution
		tempColor.rgb += uLightColor * NdotL * attenuation;
		
		vec3 reflection = reflect(-lightDir, normal);
		
		float specularStrength = dot(reflection, eye);
		specularStrength = max(specularStrength, 0.0f); // don't let it fall before zero

		// Calculate the specular contribution
		tempColor.rgb += uLightColor * pow(specularStrength, uLightSpecularExponent) * attenuation;
	}
	//outColor.rgb *= 0.001;
	//outColor.rgb += vec3(1,0,0);
	//outColor = max(attenuation * diffuse + specular, uSceneAmbient);
	outColor = outColor + tempColor;
}*/

void DirectionalLight()
{
	vec3 textureColor = texture(uScene, texcoord).rgb;
	vec3 normal = texture(uNormalMap, texcoord).xyz * 2.0 - 1.0; //Unpack
	vec3 pos = texture(uPositionMap, texcoord).xyz;
	float edgeFactor = texture(uEdgeMap, texcoord).r; //Will be zero if there is an edge

	outColor.rgb = LightAmbient;

	if(uAmbient){
		float AmbientOcclusion = texture(ssao, texcoord).r;
		//Alpha component is determined by the alpha in the texture
		outColor.rgb = LightAmbient * AmbientOcclusion;
	}

	if (length(pos) == 0.0)
	{
		discard;
	}
	

	float NdotL = dot(normal, LightDirection);
	float blocky = texture(uStepTexture, vec2(NdotL, 0.5)).r;

	/// Determine if we are shadowed ///
	vec4 shadowCoord = ViewToShadowMap * vec4(pos, 1.0);

	//Where is the current point in the map?
	//What is the depth there?
	float shadowDepth = texture(uShadowMap, shadowCoord.xy).r;

	//Is there an occluder there?
	if (NdotL < 0.0 || shadowDepth < shadowCoord.z - 0.001f)
	{
		//We have shadow!
		outColor.rgb *= 0.7;
		//You could put an actual colour here?
	}
	else if (NdotL > 0.0) //If the normal is facing the light
	{
		//The light contributes to this surface

		//Calculate diffuse contribution
		outColor.rgb += LightDiffuse * NdotL;

		//Blinn-Phong half vector
		float NdotHV = max(dot(normal, normalize(LightDirection + normalize(-pos))), 0.0);

		//Calculate specular contribution
		outColor.rgb += LightSpecular * pow(NdotHV, LightSpecularExponent);
	}

	if (uToonActive)
	{
		outColor.rgb *= textureColor.rgb * edgeFactor * blocky;
	}
	else
	{
		outColor.rgb *= textureColor.rgb;
	}
	
	outColor.a = 1.0;
}

void main()
{	
	DirectionalLight();
	//SpotLight();
}