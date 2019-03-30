#version 420

uniform sampler2D uTex;

in vec2 texcoord; 

out vec4 outColor;


void main()
{
	//vec4 source = texture(uTex, texcoord);

	float grey = dot(texture(uTex, texcoord).rgb, vec3(0.299, 0.587, 0.114));

	outColor.rgb = vec3(grey * vec3(1.2, 1.0, 0.8));
	outColor.a = 1.0f;
}