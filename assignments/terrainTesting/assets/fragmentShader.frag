#version 450
out vec4 FragColor;

//Terrain
in vec3 WorldPos;
//
in vec3 Normal;
in vec2 UV;
//Terrain
uniform sampler2D _TextureSnow;
uniform sampler2D _TextureGrass;
uniform sampler2D _TextureRock;
//

//0 for uniform color 
//1 for normals
//2 for UVs
//3 for texture
//4 for shaded
uniform int _Mode;
uniform vec3 _Color;
uniform vec3 _LightDir;
uniform float _AmbientK = 0.3;
uniform float _DiffuseK = 0.7;

//Terrain
uniform float _terMinY;
uniform float _terMaxY;

uniform float _HBTrange1;
uniform float _HBTrange2;
uniform float _HBTrange3;
uniform float _HBTrange4;
//

float calcLight(vec3 normal){
	return _AmbientK + _DiffuseK * max(dot(-_LightDir,normal),0.0);
}

//Terrain
vec4 heightBasedTexture(float scaleIn)
{
		vec4 color;

		//If vertex below range 1 texture as rock
		if (scaleIn >= 0.0 && scaleIn <= _HBTrange1)
		{
			color = texture(_TextureRock,UV);
		}
		//If vertex between range1 and range2 texture as mix between rock and grass
		else if (scaleIn <= _HBTrange2)
		{
			scaleIn -= _HBTrange1;
			scaleIn /= (_HBTrange2 - _HBTrange1);

			float scaletmp = scaleIn;
			scaleIn = 1.0f - scaleIn;

			color += texture(_TextureRock,UV) * scaleIn;
			color += texture(_TextureGrass,UV) * scaletmp;
		}
		//If vertex between range2 and range 3 texture as grass
		else if (scaleIn <= _HBTrange3)
		{
			color = texture(_TextureGrass,UV);
		}
		//If vertex between range3 and range4 texture as mix between grass and snow
		else if (scaleIn <= _HBTrange4)
		{
			scaleIn -= _HBTrange3;
			scaleIn /= (_HBTrange4 - _HBTrange3);

			float scaletmp = scaleIn;
			scaleIn = 1.0f - scaleIn;

			color += texture(_TextureGrass,UV) * scaleIn;
			color += texture(_TextureSnow,UV) * scaletmp;
		}
		//If vertex above range4 texture as snow
		else
		{
			color = texture(_TextureSnow,UV);
		}

		return color;
}
//


void main(){

	if (_Mode == 0){
		FragColor = vec4(_Color,1.0);
	}
	else if (_Mode == 1){
		vec3 normal = normalize(Normal);
		FragColor = vec4(normal,1.0);
	}
	else if (_Mode == 2){
		FragColor = vec4(UV,0.0,1.0);
	}
	else if (_Mode == 3)
	{
	//Terrain
		//Gets y cord of vertex positions and puts it on a scale 
		//of 0-1 based on how high up it is in the terrain
		float scale = abs(WorldPos.y - _terMinY) / (abs(_terMaxY - _terMinY));

		FragColor = heightBasedTexture(scale);
	//
	}else if (_Mode == 4){
		vec3 normal = normalize(Normal);
		vec3 col = _Color * calcLight(normal);
		FragColor = vec4(col,1.0);
	}else if (_Mode == 5){
		vec3 normal = normalize(Normal);
		float scale = (WorldPos.y + _terMinY) / (abs(_terMinY) + abs(_terMaxY));
		vec3 col = heightBasedTexture(scale).rgb * calcLight(normal);
		FragColor = vec4(col,1.0);
	}
}