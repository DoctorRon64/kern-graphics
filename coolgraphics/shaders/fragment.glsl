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
uniform vec3 lightPos1;
uniform vec3 lightPos2;

uniform vec3 lightColorAmbient;
uniform vec3 lightColorDiffuse;
uniform vec3 lightColorSpecular;

uniform vec3 lightColorAmbient1;
uniform vec3 lightColorDiffuse1;
uniform vec3 lightColorSpecular1;

uniform vec3 lightColorAmbient2;
uniform vec3 lightColorDiffuse2;
uniform vec3 lightColorSpecular2;

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

    // Lighting calculations for the second light source
    vec3 lightDir1 = normalize(lightPos1 - worldPos.xyz);
    vec3 reflDir1 = reflect(-lightDir1, normal);
    float lightValue1 = max(dot(normal, lightDir1), 0.0);
    float spec1 = pow(max(dot(reflDir1, viewDir), 0.0), 8);

    vec3 ambient1 = lightColorAmbient1 * texture(mainTexture, uv).rgb;
    vec3 diffuse1 = lightColorDiffuse1 * lightValue1 * texture(mainTexture, uv).rgb;
    vec3 specular1 = lightColorSpecular1 * spec1 * vec3(1.0); // Assuming white specular light

    // Lighting calculations for the third light source
    vec3 lightDir2 = normalize(lightPos2 - worldPos.xyz);
    vec3 reflDir2 = reflect(-lightDir2, normal);
    float lightValue2 = max(dot(normal, lightDir2), 0.0);
    float spec2 = pow(max(dot(reflDir2, viewDir), 0.0), 8);

    vec3 ambient2 = lightColorAmbient2 * texture(mainTexture, uv).rgb;
    vec3 diffuse2 = lightColorDiffuse2 * lightValue2 * texture(mainTexture, uv).rgb;
    vec3 specular2 = lightColorSpecular2 * spec2 * vec3(1.0); // Assuming white specular light

    // Final Color Calculation
    vec3 finalColor = ambient + diffuse + specular + ambient1 + diffuse1 + specular1 + ambient2 + diffuse2 + specular2;
    FragColor = vec4(finalColor, 1.0);
}
