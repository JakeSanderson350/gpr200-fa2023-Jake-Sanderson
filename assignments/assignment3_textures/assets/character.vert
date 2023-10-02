#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;

uniform float iTime;

void main(){
	UV = vUV;

	float x = cos(iTime) / 2.0;
	float y = sin(iTime) / 2.0;

	gl_Position = vec4(vPos.x + x, vPos.y + y, vPos.z, 1.0);
}