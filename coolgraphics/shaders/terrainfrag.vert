#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec4 worldPos;

uniform sampler2D mainTexture;
uniform sampler2D normalMap;

uniform sampler2D dirt, sand, grass, rock, snow;

uniform vec3 lightDir;
uniform vec3 camPos;

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}

void main()
{
    // Normal Map
    vec3 normal = texture(normalMap, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal.gb = normal.bg;
    normal.r = -normal.r;
    normal.b = -normal.b;

    // Lighting calculations for the first light source
    //vec3 viewDir = normalize(camPos - worldPos.xyz);
    //vec3 reflDir = reflect(-lightDir, normal);

    float lightValue = max(-dot(normal, lightDir), 0.0);
    //float spec = pow(max(dot(reflDir, viewDir), 0.0), 8);

    //build color
    float y = worldPos.y;

    float dist = length(worldPos.xyz - camPos);
    //blend 250
    float uvLerp = clamp((dist - 250) / 159, -1, 1) * .5 + .5;

    //height 50 and widht 10
    float ds = clamp((y-10) / 10, -1, 1) * .5 + .5;
    float sg = clamp((y-20) / 10, -1, 1) * .5 + .5;
    float gr = clamp((y-30) / 10, -1, 1) * .5 + .5;
    float rs = clamp((y-50) / 10, -1, 1) * .5 + .5;

    vec3 dirtColorClose = texture(dirt, uv * 100).rgb;
    vec3 sandColorClose = texture(sand, uv * 100).rgb;
    vec3 grassColorClose = texture(grass, uv * 100).rgb;
    vec3 rockColorClose = texture(rock, uv * 100).rgb;
    vec3 snowColorClose = texture(snow, uv * 100).rgb;
    
    vec3 dirtColorFar = texture(dirt, uv * 10).rgb;
    vec3 sandColorFar = texture(sand, uv * 10).rgb;
    vec3 grassColorFar = texture(grass, uv * 10).rgb;
    vec3 rockColorFar = texture(rock, uv * 10).rgb;
    vec3 snowColorFar = texture(snow, uv * 10).rgb;

    vec3 dirtColor = lerp(dirtColorClose, dirtColorFar, uvLerp);
    vec3 sandColor = lerp(sandColorClose, sandColorFar, uvLerp);
    vec3 grassColor = lerp(grassColorClose, grassColorFar, uvLerp);
    vec3 rockColor = lerp(rockColorClose, rockColorFar, uvLerp);
    vec3 snowColor = lerp(snowColorClose, snowColorFar, uvLerp);

    vec3 diffuse = lerp(lerp(lerp(lerp(dirtColor, sandColor, ds), grassColor, sg), rockColor, gr), snowColor, rs);

    // Final Color Calculation
    vec4 outputCol = vec4(diffuse * min(lightValue + 0.1, 1.0), 1.0);// + spec * outputColor.rgb;

    FragColor = outputCol;
}
