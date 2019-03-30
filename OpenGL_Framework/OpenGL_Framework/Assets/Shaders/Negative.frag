#version 420

uniform sampler2D uTex;

in vec2 texcoord; 

out vec4 outColor;


void main()
{
	vec4 source = texture(uTex, texcoord);


	outColor.rgb = vec3(1.0f -source.rgb);
	outColor.a = 1.0f;
}