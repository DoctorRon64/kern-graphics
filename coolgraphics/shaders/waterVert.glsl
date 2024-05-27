#version 330 core
layout (location = 0) in vec3 vPos;

out vec4 worldPos;
out vec2 uv;

uniform mat4 world, view, projection;
uniform float waterHeight;

void main() 
{
    worldPos = world * vec4(vPos, 1.0);
	worldPos.y = waterHeight;
    gl_Position = projection * view * worldPos;
    uv = (gl_Position.xy / gl_Position.w) * .5 + .5;
}