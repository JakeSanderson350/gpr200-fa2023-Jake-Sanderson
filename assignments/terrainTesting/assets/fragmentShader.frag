#version 450
out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;
in vec2 UV;
uniform sampler2D _TextureSnow;
uniform sampler2D _TextureGrass;
uniform sampler2D _TextureRock;

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

float calcLight(vec3 normal){
	return _AmbientK + _DiffuseK * max(dot(-_LightDir,normal),0.0);
}


vec4 heightBasedTexture(float scale)
{
		vec4 color;
		
		const float range1 = 0.15f;
		const float range2 = 0.3f;
		const float range3 = 0.65f;
		const float range4 = 0.85f;

		if (scale >= 0.0 && scale <= range1)
		{
			color = texture(_TextureRock,UV);
		}
		else if (scale <= range2)
		{
			scale -= range1;
			scale /= (range2 - range1);

			float scaletmp = scale;
			scale = 1.0f - scale;

			color += texture(_TextureRock,UV) * scale;
			color += texture(_TextureGrass,UV) * scaletmp;
		}
		else if (scale <= range3)
		{
			color = texture(_TextureGrass,UV);
		}
		else if (scale <= range4)
		{
			scale -= range3;
			scale /= (range4 - range3);

			float scaletmp = scale;
			scale = 1.0f - scale;

			color += texture(_TextureGrass,UV) * scale;
			color += texture(_TextureSnow,UV) * scaletmp;
		}
		else
		{
			color = texture(_TextureSnow,UV);
		}

		return color;
}


void main(){

	if (_Mode == 0){
		FragColor = vec4(_Color,1.0);
	}
	else if (_Mode == 1){
		vec3 normal = normalize(Normal);
		FragColor = vec4(abs(normal),1.0);
	}
	else if (_Mode == 2){
		FragColor = vec4(UV,0.0,1.0);
	}
	else if (_Mode == 3)
	{
		//16 is lowest y value, 62 is range from min to max y value
		//TODO set these as uniforms
		float scale = (WorldPos.y + 16) / 62.0f;

		FragColor = heightBasedTexture(scale);
	}else if (_Mode == 4){
		vec3 normal = normalize(Normal);
		vec3 col = _Color * calcLight(normal);
		FragColor = vec4(col,1.0);
	}else if (_Mode == 5){
		vec3 normal = normalize(Normal);
		vec3 col = texture(_TextureSnow,UV).rgb * calcLight(normal);
		FragColor = vec4(col,1.0);
	}
}