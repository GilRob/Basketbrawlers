#version 420

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

layout(location = 0) in vec4 in_vert;
layout(location = 1) in vec4 in_uv;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec4 in_vert2;
layout(location = 4) in vec4 in_uv2;
layout(location = 5) in vec4 in_normal2;
/*layout(location = 6) in vec4 in_vert3;
layout(location = 7) in vec4 in_uv3;
layout(location = 8) in vec4 in_normal3;
layout(location = 9) in vec4 in_vert4;
layout(location = 10) in vec4 in_uv4;
layout(location = 11) in vec4 in_normal4;*/

uniform float interp = 0.5;
uniform int index = 0; 

out vec2 texcoord;
out vec3 norm;
out vec3 pos;

vec3 catmull(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
{
	return 0.5f * (
		t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
		t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
		t * (-p0 + p2) +
		(2.0f * p1)		);
}

vec4 catmull(vec4 p0, vec4 p1, vec4 p2, vec4 p3, float t)
{
	return 0.5f * (
		t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
		t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
		t * (-p0 + p2) +
		(2.0f * p1)		);
}

void main()
{
	texcoord = in_uv.xy;
	vec4 interpNorm;

	if (index == 0)
		interpNorm = mix(in_normal, in_normal2, interp);
	else if (index == 1)
		interpNorm = mix(in_normal2, in_normal, interp);
	/*else if (index == 2)
		interpNorm = mix(in_normal2, in_normal3, in_normal4, in_normal, interp);
	else if (index == 3)
		interpNorm = mix(in_normal3, in_normal4, in_normal, in_normal2, interp);*/
	
	norm = mat3(uView) * mat3(uModel) * interpNorm.xyz;
	vec4 cat; 
	//
	//if (index == 0)
	//	cat = catmull(in_vert4, in_vert, in_vert2, in_vert3, interp);
	//else if (index == 1)
	//	cat = catmull(in_vert, in_vert2, in_vert3, in_vert4, interp);
	//else if (index == 2)
	//	cat = catmull(in_vert2, in_vert3, in_vert4, in_vert, interp);
	//else if (index == 3)
	//	cat = catmull(in_vert3, in_vert4, in_vert, in_vert2, interp);

	//Technically shouldnt have this in vertext shader
	//This is for LERP
	if (index == 0)
		cat = mix(in_vert, in_vert2, interp);
	else if (index == 1)
		cat = mix(in_vert2, in_vert, interp);
	/*else if (index == 2)
		cat = mix(in_vert3, in_vert, interp);
	else if (index == 3)
		cat = mix(in_vert4, in_vert, interp);*/
	else cat = in_vert;
	pos = (uView * uModel * vec4(cat)).xyz;

	gl_Position = uProj * vec4(pos, 1.0);
}
