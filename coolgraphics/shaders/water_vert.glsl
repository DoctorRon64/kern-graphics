#version 330 core
layout (location = 0) in vec3 vPos;

out vec4 ScreenPos;
out vec4 worldPos;
out vec2 uv;

uniform mat4 world, view, projection;
uniform float waterHeight;

void main() 
{
    worldPos = world * vec4(vPos, 1.0);
    worldPos.y = waterHeight;
    ScreenPos = projection * view * worldPos;
    gl_Position = ScreenPos;
    uv = (ScreenPos.xy / ScreenPos.w) * 0.5 + 0.5;
}