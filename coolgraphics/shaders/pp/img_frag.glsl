#version 330 core
out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D MainTex;

void main() 
{
	FragColor = vec4(texture(MainTex, fragCoord).rgb, 1);
}