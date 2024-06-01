#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

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
    vec4 diffuse = 1 - texture(texture_diffuse1, TexCoords);
    vec4 specTex = texture(texture_specular1, TexCoords);

    float light = max(dot(-lightDir, Normals), 0.0);

    vec3 viewDir = normalize(FragPos.rgb - camPos);
    vec3 refl = reflect(lightDir, Normals);

    float ambientOcclusion = texture(texture_ao1, TexCoords).r;
    
    float roughness = texture(texture_roughness1, TexCoords).r;
    float spec = pow(max(dot(-viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb;

    //fog
    vec3 topColor = rgb(68.0, 118.0, 189.0);
    vec3 bottomColor = rgb(188.0 , 214.0, 231.0);

    float dist = length(FragPos.xyz - camPos);
    float fog = pow(clamp((dist - 250) / 1000, 0, 1),2);
    vec3 fogColor = lerp(bottomColor, topColor, max(viewDir.y, 0.0));
    
    vec4 finalOutput = lerp(diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0), vec4(fogColor, 1.0), fog);
    //if (finalOutput.a < 0.5) discard; //alpha clipping
    FragColor = finalOutput;
}