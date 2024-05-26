#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 DepthColor;

in vec4 worldPos;

uniform vec3 lightDir;
uniform vec3 camPos;

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

vec3 rgb(float r, float g, float b) {
    return vec3(r / 255.0, g / 255.0, b / 255.0);
}

void main()
{
    vec3 topColor = rgb(68.0, 118.0, 189.0);
    vec3 bottomColor = rgb(210.0, 232.0, 247.0);
    vec3 viewDir = normalize(worldPos.xyz - camPos);

    vec3 lightDirection = normalize(lightDir);
    vec3 sunColor = rgb(255.0, 200.0, 50.0);
    float sun = max(pow(dot(-viewDir, lightDirection), 128), 0.0);
    
    FragColor = vec4(lerp(bottomColor, topColor, abs(viewDir.y)) + sun * sunColor, 1);
    DepthColor = vec4(0);
}
