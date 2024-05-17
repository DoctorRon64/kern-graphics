#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUv;
layout (location = 2) in vec3 vNormal;

out vec2 uv;
out vec3 normal;
out vec4 worldPos;

uniform mat4 world, view, projection;

void main()
{
    //inverse order object > world > camera > projection
    gl_Position = projection * view * world * vec4(vPos, 1.0f);
    uv = vUv;
    normal = vNormal;

    worldPos = world * vec4(vPos, 1.0);
}