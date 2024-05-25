#version 330 core
out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D MainTex;

void main() 
{
	float rDisplace = 0.95;
	float gDisplace = 0.97;
	float bDisplace = 1.0;

	vec2 Displace = fragCoord - vec2(0.5,0.5);

	float r = texture(MainTex, vec2(0.5,0.5) + Displace * rDisplace).r;
	float g = texture(MainTex, vec2(0.5,0.5) + Displace * gDisplace).g;
	float b = texture(MainTex, vec2(0.5,0.5) + Displace * bDisplace).b;

	FragColor = vec4(r, g, b, 1.0);
}