#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;

out vec2 uv;
out vec3 normal;
out vec4 worldPos;

uniform mat4 world, view, projection;
uniform sampler2D mainTexture;

void main()
{
    vec3 pos = vPos;
    //object space offset

    worldPos = world * vec4(vPos, 1.0);
    //worldSpace
    worldPos.y += texture(mainTexture, vUv).r * 120.0;

    gl_Position = projection * view * worldPos;
    uv = vUv;
    normal = vNormal;
}