#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in mat3 tbn;
in vec4 worldPos;

uniform sampler2D mainTexture;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 camPos;
uniform vec4 lightColor;  // New uniform for light color

void main()
{
    // Normal Map
    vec3 normal = texture(normalMap, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    //tweak normal map
    normal.rg = normal.rg * .75;
    normal = normalize(normal);
    normal = tbn * normal;

    // Lighting
    vec3 lightDir = normalize(worldPos.xyz - lightPos);
    vec3 viewDir = normalize(worldPos.xyz - camPos);
    vec3 reflDir = normalize(-reflect(-lightDir, normal));
    float lightValue = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(-dot(reflDir, normalize(viewDir)), 0.0), 8); //higher power = smaller highlight

    // Color
    vec4 clrOutput = vec4(color, 1.0f) * texture(mainTexture, uv);
    clrOutput.rgb = clrOutput.rgb * min(lightValue + 0.1f, 1.0f) + spec * clrOutput.rgb;
    clrOutput.rgb *= lightColor.rgb;
    FragColor = clrOutput;
}
