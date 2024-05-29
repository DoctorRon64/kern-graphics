#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughness;
uniform sampler2D texture_ao;

uniform vec3 camPos;
uniform vec3 lightDir;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void main()
{
    vec4 diffuse = texture(texture_diffuse, TexCoords);
    vec4 specTex = texture(texture_specular, TexCoords);

    float light = max(dot(-lightDir, Normals), 0.0);

    vec3 viewDir = normalize(camPos - FragPos.rgb);
    vec3 refl = reflect(lightDir, Normals);

    float ambientOcclusion = texture(texture_ao, TexCoords).r;
    
    float roughness = texture(texture_roughness, TexCoords).r;
    float spec = pow(max(dot(viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb;
    
    FragColor = diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0);
}