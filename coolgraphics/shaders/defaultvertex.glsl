#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vNormal;
layout (location = 4) in vec3 vTangent;
layout (location = 5) in vec3 vBitangent;

out vec3 color;
out vec2 uv;
out mat3 tbn;
out vec4 worldPos;

uniform mat4 world, view, projection;

void main()
{
    //inverse order object > world > camera > projection
    gl_Position = projection * view * world * vec4(vPos, 1.0f);
    color = vColor;
    uv = vUv;

    vec3 normal = normalize( mat3(world) * vNormal);
    vec3 tangent = normalize( mat3(world) * vTangent);
    vec3 bitangent = normalize( mat3(world) * vBitangent);
    tbn = mat3(tangent, bitangent, normal);
    
    worldPos = world * vec4(vPos, 1.0);
}
