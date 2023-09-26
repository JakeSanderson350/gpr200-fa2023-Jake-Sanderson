#version 450
out vec4 FragColor;
in vec2 UV;
uniform float iTime;
uniform vec2 iResolution;
uniform vec3 SkyBottomColor;
uniform vec3 SkyTopColor;
uniform vec3 SunColor;
uniform float SunRad;
uniform float SunSpeed;
uniform vec3 Hill1Color;
uniform vec3 Hill2Color;
	
float circleSDF(vec2 p, float r)
{
	return length(p)-r;
}
	
void main()
{
	vec2 uv = UV;

	//BG
    vec3 bottomColor = SkyBottomColor;
    vec3 topColor = SkyTopColor;
    float weight = (3.0 * sin(iTime * 0.5 * SunSpeed) + 1.0) * uv.y;
    
    vec3 sunsetBG = mix(topColor, bottomColor, weight);
    
    //change to -1 to 1
    uv = uv * 2.0 - 1.0;
    uv.x*=iResolution.x / iResolution.y * 1.0;
    
    //Sun
    vec2 circlePos = vec2(0.0,sin(iTime * 0.5 * SunSpeed) * 2.0 - 2.0);
    float circleRadius = SunRad;
    float d = circleSDF(uv - circlePos,circleRadius);
    d = 1.0 - smoothstep(-0.05, 0.05, d);
    vec3 sunColor = SunColor;
    vec3 circle = vec3(d ,d ,d);
    sunsetBG = mix(sunsetBG, sunColor, d);
    
    //FG
    float hills = 1.0 - step(-0.7 + sin(uv.x*2.0) * 0.2 + 0.1,uv.y);
    float hills2 = 1.0 - step(-0.7 + cos(uv.x*2.0) * 0.2 + 0.1,uv.y);
    
    vec3 sunsetFG = mix(sunsetBG, Hill1Color, hills);
    sunsetFG = mix(sunsetFG, Hill2Color, hills2);

    // Output to screen
    FragColor = vec4(sunsetFG, 1.0);
}