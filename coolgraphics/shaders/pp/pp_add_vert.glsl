#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 fragCoord;

void main()
{
    fragCoord = texCoords;
    gl_Position = vec4(position, 1.0);
}