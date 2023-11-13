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

uniform Light _Light;
uniform Material _Material;
uniform vec3 camPos;
uniform sampler2D _Texture;

void main()
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec4 textureTmp = texture(_Texture,fs_in.UV);

	//Diffuse lighting omega
	vec3 w = normalize(_Light.position - fs_in.WorldPos);
	//Specular lighting
	vec3 v = normalize(camPos - fs_in.WorldPos);
	vec3 h = normalize((w + v) / length(w + v));

	//Light calculation
	vec3 lightColor = (_Light.color * _Material.ambientK) + 
					  (_Light.color * (_Material.diffuseK * max(dot(normal, w), 0))) + 
					  (_Light.color * (_Material.specular * pow(max(dot(h, normal), 0), _Material.shininess)));

	FragColor = vec4(textureTmp.rgb * lightColor, 1.0f);
}