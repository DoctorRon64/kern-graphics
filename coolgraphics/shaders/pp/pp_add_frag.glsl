#version 330 core

out vec4 FragColor;
in vec2 fragCoord;

uniform sampler2D framebuffer1;
uniform sampler2D framebuffer2;
uniform float bloomintensity;

void main() {
    vec4 color = vec4(0.0);

    color += texture(framebuffer1, fragCoord);
    color += texture(framebuffer2, fragCoord) * bloomintensity;

    //FragColor = texture(framebuffers[0], fragCoord);

    FragColor = color;
}