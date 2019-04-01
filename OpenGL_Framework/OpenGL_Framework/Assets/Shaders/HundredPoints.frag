#version 420

uniform sampler2D uSceneAlbedo;
uniform sampler2D uNormalMap;
uniform sampler2D uPositionMap;

struct Light{
	vec3 Position;
	vec3 Color;
	float Radius;
};

const int NUM_LIGHTS = 100;
uniform Light lights[NUM_LIGHTS];
uniform vec3 viewPos;

in vec2 texcoord;

out vec4 outColor;

void main()
{
	vec3 Diffuse = texture(uSceneAlbedo, texcoord).rgb;
	vec3 Normal = texture(uNormalMap, texcoord).xyz * 2.0 - 1.0; //Unpack
	vec3 FragPos = texture(uPositionMap, texcoord).xyz;
	float Specular = texture(uSceneAlbedo, texcoord).a;

	vec3 lighting = Diffuse*0.1;//ambient calculation
	vec3 viewDir = normalize(viewPos - FragPos);

	float linear = 0.7f;
	float quadratic = 1.8f;

	for(int i = 0; i < NUM_LIGHTS; i++){
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);

        if(distance < lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].Color * spec * Specular;
            // attenuation
            float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }    
    outColor = vec4(lighting, 1.0);
}
