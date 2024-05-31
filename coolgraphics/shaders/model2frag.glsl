#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughness;

uniform vec3 camPos;
uniform vec3 lightDir;

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}
vec4 lerp(vec4 a, vec4 b, float t) {
    return a + (b - a) * t;
}
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

vec3 rgb(float r, float g, float b) {
    return vec3(r / 255.0, g / 255.0, b / 255.0);
}

void main()
{
    vec4 diffuse = texture(texture_diffuse, TexCoords);
    vec4 specTex = texture(texture_specular, TexCoords);

    // Sample the normal map
    vec3 normal = normalize(texture(texture_normal, TexCoords).rgb * 2.0 - 1.0);
    vec3 lightDirNorm = normalize(lightDir);
    
    // Compute the lighting using the normal from the normal map
    float light = max(dot(-lightDirNorm, normal), 0.0);

    vec3 viewDir = normalize(FragPos.rgb - camPos);
    vec3 refl = reflect(lightDirNorm, normal);

    float roughness = texture(texture_roughness, TexCoords).r;
    float spec = pow(max(dot(-viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb;

    // Fog
    vec3 topColor = rgb(68.0, 118.0, 189.0);
    vec3 bottomColor = rgb(188.0, 214.0, 231.0);

    float dist = length(FragPos.xyz - camPos);
    float fog = pow(clamp((dist - 250) / 1000, 0, 1), 2);
    vec3 fogColor = lerp(bottomColor, topColor, max(viewDir.y, 0.0));

    vec4 finalOutput = diffuse * light + vec4(specular, 0);
    FragColor = lerp(finalOutput, vec4(fogColor, 1.0), fog);
}
