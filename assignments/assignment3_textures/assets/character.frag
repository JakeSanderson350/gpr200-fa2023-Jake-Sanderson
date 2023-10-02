#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _CharacterTexture;
uniform float iTime;


void main(){
	vec4 color = texture(_CharacterTexture, UV);
	
	float t = abs(sin(iTime));

	FragColor = vec4(color.rgb, color.a * t);
}