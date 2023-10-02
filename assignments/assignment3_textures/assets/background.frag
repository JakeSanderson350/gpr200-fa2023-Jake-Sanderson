#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _BackgroundTexture;
uniform sampler2D _NoiseTexture;


void main(){
	FragColor = texture(_BackgroundTexture, UV);
	//FragColor = vec4(UV, 0.0, 1.0);
}