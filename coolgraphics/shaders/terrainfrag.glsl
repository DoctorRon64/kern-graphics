#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec4 worldPos;

uniform sampler2D mainTexture;
uniform sampler2D normalMap;

uniform vec3 lightDir;
uniform vec3 camPos;

void main()
{
    // Normal Map
    //vec3 normal = texture(normalMap, uv).rgb;
    //normal = normalize(normal * 2.0 - 1.0);

    // Lighting calculations for the first light source
    //vec3 viewDir = normalize(camPos - worldPos.xyz);
    //vec3 reflDir = reflect(-lightDir, normal);

    float lightValue = max(-dot(normal, lightDir), 0.0);
    //float spec = pow(max(dot(reflDir, viewDir), 0.0), 8);

    // Final Color Calculation
    vec4 output = texture(mainTexture, uv);
    output.rgb = output.rgb * min(lightValue + 0.1, 1.0);// + spec * outputColor.rgb;
    FragColor = output;
}
