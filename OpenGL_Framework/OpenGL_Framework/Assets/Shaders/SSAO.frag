#version 420 core
out float outColor;
in vec2 texcoord;

uniform sampler2D uNormalTex;
uniform sampler2D uTexNoise;
uniform sampler2D uDepthMap;

uniform vec2 noiseScale;
uniform vec3 samples[16];
uniform mat4 projection;
uniform mat4 uProjBiasMatrixInverse;

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 16;
float radius = 0.100;
float bias = 0.025;



void main()
{
    // get input for SSAO algorithm
	float depth =  texture(uDepthMap, texcoord).r;
	if(depth == 1.0)
	{
		discard;
	}
	vec4 fragPos = uProjBiasMatrixInverse * vec4(texcoord, depth, 1.0);
	fragPos /= fragPos.w;

	vec3 normal = texture(uNormalTex, texcoord).rgb;
	normal = normal + normal - 1.0;
	vec3 randomVec = texture(uTexNoise, texcoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - (normal * dot(randomVec, normal)));
		vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	// Iterate over the sample kernel and calculate occlusion factor
	float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
		samplePos = fragPos.xyz + samplePos * (radius*depth); 

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
		offset.xy /= offset.w; // perspective divide
		offset.xy = offset.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0

        
		float sampleDepth = texture(uDepthMap, offset.xy).r;
		vec4 sampleDepthToPos = uProjBiasMatrixInverse * vec4(texcoord, sampleDepth, 1.0);
		sampleDepthToPos /= sampleDepthToPos.w;

		float rangeCheck = 1.0;
		// range check & accumulate		
		rangeCheck = abs(samplePos.z - sampleDepthToPos.z) < radius ? 1.0 : 0.0;
		occlusion += (sampleDepthToPos.z > samplePos.z ? 1.0 : 0.0) * rangeCheck;		
	}
	occlusion = 1.0 - ((occlusion * 1.25 * (1.5-depth)) / kernelSize);
    
	outColor.r = occlusion;
}