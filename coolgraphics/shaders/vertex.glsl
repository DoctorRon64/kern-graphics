#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vNormal;

out vec3 color;
out vec2 uv;
out vec3 normal;
out vec4 worldPos;

uniform mat4 world, view, projection;

void main()
{
    //inverse order object > world > camera > projection
    gl_Position = projection * view * world * vec4(vPos, 1.0f);
    color = vColor;
    uv = vUv;
    normal = mat3(world) * vNormal;

    worldPos = world * vec4(vPos, 1.0);
}
