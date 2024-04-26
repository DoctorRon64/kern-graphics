#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in vec3 normal;
in vec4 worldPos;

uniform sampler2D mainTexture;
uniform vec3 lightPos;

void main()
{
    vec3 lightDirection = normalize(worldPos.xyz - lightPos);
    float lightValue = max(-dot(normal, lightDirection), 0.0f);

    FragColor = vec4(color, 1.0f) * texture(mainTexture, uv) * min(lightValue + 0.1f, 1.0f);
}
