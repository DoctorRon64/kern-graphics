#version 330 core
out vec4 FragColor;

in vec4 worldPos;

uniform vec3 lightDir;
uniform vec3 camPos;
uniform vec3 sunColor;

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

void main()
{
    vec3 topColor = vec3(68.0 / 255.0, 118.0 / 255.0 , 189.0 / 255.0);
    vec3 bottomColor = vec3(188.0 / 255.0,  214.0 / 255.0, 231.0 / 255.0);
    vec3 sunColor = vec3(1.0, 200 / 255.0, 50 / 255.0);

    vec3 viewDir = normalize(worldPos.xyz - camPos);
    float sun = max(pow(dot(-viewDir, lightDir), 128), 0.0);
    
    FragColor = vec4(lerp(bottomColor, topColor, abs(viewDir.y)) + sun * sunColor, 1);
}
