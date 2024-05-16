#version 330 core
layout (location = 0) in vec3 vPos;

out vec4 worldPos;
uniform mat4 world, view, projection;

void main()
{
    //inverse order object > world > camera > projection
    gl_Position = projection * view * world * vec4(vPos, 1.0f);
    worldPos = world * vec4(vPos, 1.0);
}
