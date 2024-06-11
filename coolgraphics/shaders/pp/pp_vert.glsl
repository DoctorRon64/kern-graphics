#version 330 core
layout (location = 0) in vec3 vPos;

out vec2 fragCoord;

void main() {
	gl_Position = vec4(vPos, 1.0);
	fragCoord = (gl_Position.xy / gl_Position.w) *.5 +.5;
}