#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 DepthColor;

in vec4 worldPos;
in vec2 uv;
in vec4 ScreenPos;

uniform sampler2D color;
uniform sampler2D depth;
uniform sampler2D invert;
uniform sampler2D normalMap;

uniform vec3 lightDir;
uniform vec3 camPos;
uniform float time;

vec2 lerp(vec2 a, vec2 b, float t) {
    return a + (b - a) * t;
}

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

vec4 lerp(vec4 a, vec4 b, float t) {
    return a + (b - a) * t;
}

vec3 rgb(float r, float g, float b) {
    return vec3(r / 255.0, g / 255.0, b / 255.0);
}

void main() 
{
    vec3 waterColor = rgb(0, 20, 32);

    // Recalculate screen UV in the fragment shader
    vec2 screenUV = (ScreenPos.xy / ScreenPos.w) * 0.5 + 0.5;

    // Invert x pos
    vec2 invertUv = screenUV;
    invertUv.x = 1.0 - invertUv.x;

    // Screen edge disglithify
    float edgy = (1.0 - pow(abs(0.5 - screenUV.x) * 2.0, 8.0)) * (1.0 - pow(abs(0.5 - screenUV.y) * 2.0, 8.0));
    float depthValue = distance(camPos, worldPos.xyz) / 100.0;
    float sceneDepth = texture(depth, screenUV).r;
    float deepness = sceneDepth - depthValue;

    vec2 waterUv1 = vec2(worldPos.x * 14.1 + time * .25, worldPos.z * -24.0 + time * .58);
    vec2 waterUv2 = vec2(worldPos.x * 17.0 + time * .35, worldPos.z * 14.0 + time * .23);
    vec2 waterUv3 = vec2(worldPos.x * 10.3 + time * .45, worldPos.z * 14.0 + time * .32);
    vec2 waterUv4 = vec2(worldPos.x * 25.0 + time * .55, worldPos.z * 14.0 + time * .25);

    // Calculate normal stuff
    vec2 normal;
    normal += texture(normalMap, waterUv1).rg * 2.0 - 1.0;
    normal += texture(normalMap, waterUv2).rg * 2.0 - 1.0;
    normal += texture(normalMap, waterUv3).rg * 2.0 - 1.0;
    normal += texture(normalMap, waterUv4).rg * 2.0 - 1.0;
    normal /= 4.0; //< -- hoeveel heid ingelezen
    normal *= 0.1 * edgy * min(deepness, 1.0);

    vec3 uvColor = texture(color, screenUV + normal).rgb;
    uvColor = lerp(uvColor, waterColor, min(deepness * 4, 1.0));
    vec3 reflectColor = texture(invert, invertUv + normal).rgb;

    vec3 viewDir = normalize(worldPos.xyz - camPos);
    float fresnel = max(dot(-viewDir, vec3(0.0, 1.0, 0.0)), 0.0);

    vec3 surfaceBlend = lerp(reflectColor, uvColor, fresnel);

    //specular highlight!
    vec3 reflNormal = vec3(0, 1, 0);
    reflNormal.rg += normal;
    vec3 refl = reflect(lightDir, normalize(reflNormal));
    float spec = pow(max(dot(-refl, viewDir), 0.0), 208);

    //calculate fog
    vec3 topColor = rgb(68.0, 118.0, 189.0);
    vec3 bottomColor = rgb(188.0 , 214.0, 231.0);
    float dist = length(worldPos.xyz - camPos);
    float fog = pow(clamp((dist - 250) / 1000, 0, 1),2);
    vec3 fogColor = lerp(bottomColor, topColor, max(viewDir.y, 0.0));

    // Output final color
    vec4 finalColor = vec4(surfaceBlend + vec3(spec), 1.0);
    FragColor = lerp(finalColor, vec4(fogColor, 1.0), fog);
    DepthColor = vec4(1.0, 0.0, 0.0, 1.0);
}
