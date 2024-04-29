#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in mat3 tbn;
in vec4 worldPos;

uniform sampler2D normalMap;
uniform sampler2D mainTexture;

uniform vec3 lightPos;

void main()
{
    //Normal Map
    vec3 normal = texture(normalMap, uv).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    normal = tbn * normal;

    //Lighting
    vec3 lightDirection = normalize(worldPos.xyz - lightPos);
    float lightValue = max(dot(normal, lightDirection), 0.0f);

    FragColor = vec4(color, 1.0f) * texture(mainTexture, uv) * min(lightValue + 0.1f, 1.0f);
    //FragColor = texture(normalMap, uv);
}
