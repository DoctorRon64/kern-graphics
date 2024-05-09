#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in mat3 tbn;
in vec4 worldPos;

uniform sampler2D mainTexture;
uniform sampler2D normalMap;

uniform vec3 camPos;
uniform vec3 lightPos;

uniform vec3 lightColorAmbient;
uniform vec3 lightColorDiffuse;
uniform vec3 lightColorSpecular;

void main()
{
    // Normal Map
    vec3 normal = texture(normalMap, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal.rg = normal.rg * .75;
    normal = normalize(normal);
    normal = tbn * normal;

    // Lighting calculations for the first light source
    vec3 lightDir = normalize(lightPos - worldPos.xyz);
    vec3 viewDir = normalize(camPos - worldPos.xyz);
    vec3 reflDir = reflect(-lightDir, normal);
    float lightValue = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(reflDir, viewDir), 0.0), 8);

    vec3 ambient = lightColorAmbient * texture(mainTexture, uv).rgb;
    vec3 diffuse = lightColorDiffuse * lightValue * texture(mainTexture, uv).rgb;
    vec3 specular = lightColorSpecular * spec * vec3(1.0); // Assuming white specular light

    // Final Color Calculation
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, 1.0);
}
