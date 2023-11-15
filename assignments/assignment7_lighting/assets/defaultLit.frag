#version 450
out vec4 FragColor;

in Surface
{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormal;
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};

struct Material
{
	float ambientK; //Ambient coefficient (0-1)
	float diffuseK; //Diffuse coefficient (0-1)
	float specular; //Specular coefficient (0-1)
	float shininess; //Shininess
};

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];
uniform int numLights;
uniform Material _Material;
uniform vec3 camPos;
uniform sampler2D _Texture;

void main()
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec4 textureTmp = texture(_Texture,fs_in.UV);
	vec3 lightColor = vec3(0.0f);
	
	//Specular lighting v
	vec3 v = normalize(camPos - fs_in.WorldPos);
	
	for (int i = 0; i < numLights; i++)
	{
		//Diffuse lighting omega
		vec3 w = normalize(_Lights[i].position - fs_in.WorldPos);
		//Specular lighting h
		vec3 h = normalize((w + v) / length(w + v));

		//Light calculation
		lightColor += (_Lights[i].color * _Material.ambientK);
		lightColor += (_Lights[i].color * (_Material.diffuseK * max(dot(normal, w), 0)));
		lightColor += (_Lights[i].color * (_Material.specular * pow(max(dot(h, normal), 0), _Material.shininess)));
	}

	FragColor = vec4(textureTmp.rgb * lightColor, 1.0f);
}