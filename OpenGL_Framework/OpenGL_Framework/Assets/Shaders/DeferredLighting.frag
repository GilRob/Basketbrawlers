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
//uniform sampler2D uEdgeMap;
//uniform sampler2D uStepTexture;

in vec2 texcoord;

out vec4 outColor;

void main()
{	
	/// Get Data ///
	vec3 textureColor = texture(uScene, texcoord).rgb;
	vec3 normal = texture(uNormalMap, texcoord).xyz * 2.0 - 1.0; //Unpack
	vec3 pos = texture(uPositionMap, texcoord).xyz;
	//float edgeFactor = texture(uEdgeMap, texcoord).r; //Will be zero if there is an edge

	if (length(pos) == 0.0)
	{
		discard;
	}
	
	//Alpha component is determined by the alpha in the texture
	outColor.rgb = LightAmbient;

	float NdotL = dot(normal, LightDirection);
	//float blocky = texture(uStepTexture, vec2(NdotL, 0.5)).r;

	/// Determine if we are shadowed ///
	vec4 shadowCoord = ViewToShadowMap * vec4(pos, 1.0);

	//Where is the current point in the map?
	//What is the depth there?
	float shadowDepth = texture(uShadowMap, shadowCoord.xy).r;

	//Is there an occluder there?
	if (shadowDepth < shadowCoord.z - 0.001f)
	{
		//We have shadow!
		outColor.rgb *= 0.5;
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

	outColor.rgb *= textureColor.rgb; //* edgeFactor * blocky;

	outColor.a = 1.0;

}