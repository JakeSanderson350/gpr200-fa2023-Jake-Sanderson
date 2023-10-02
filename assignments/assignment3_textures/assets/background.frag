#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _BackgroundTexture;
uniform sampler2D _NoiseTexture;
uniform float iTime;
uniform float iTile;

void main(){
	
	vec4 noiseColor = texture(_NoiseTexture, UV);

	float noise = texture(_NoiseTexture,UV).r;
	vec2 uv = iTile * UV + noise + iTime * 0.1f;
	vec4 bgColor = texture(_BackgroundTexture, uv);

	
	//FragColor = texture(_BackgroundTexture, UV);
	FragColor = bgColor;
}