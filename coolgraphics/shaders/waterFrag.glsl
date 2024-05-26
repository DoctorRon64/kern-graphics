#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 DepthColor;

in vec4 worldPos;
in vec2 uv;

uniform sampler2D color;
uniform sampler2D depth;
uniform sampler2D invert;

uniform vec3 lightDir;
uniform vec3 camPos;

void main() 
{
	FragColor = vec4(1);
	DepthColor = vec4(1, 0, 0 ,1);
}