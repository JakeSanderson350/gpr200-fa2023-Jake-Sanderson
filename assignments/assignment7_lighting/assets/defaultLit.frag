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
uniform sampler2D _Texture;

void main()
{
	vec3 normal = normalize(fs_in.WorldNormal);

	//Diffuse lighting omega
	vec3 w = normalize(_Light.position - fs_in.WorldPos);

	FragColor = texture(_Texture,fs_in.UV) * ((_Material.diffuseK * max(dot(normal, w), 0)));
}