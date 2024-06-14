#version 330 core

out vec4 FragColor;

in vec2 fragCoord;

uniform sampler2D framebuffers[10];
uniform int numFramebuffers;

void main() {
    vec4 color = vec4(0.0);

    for (int i = 0; i < numFramebuffers; i++) {
        color += texture(framebuffers[i], fragCoord);
    }
    FragColor = texture(framebuffers[0], fragCoord);

    //FragColor = color / float(numFramebuffers);
}
