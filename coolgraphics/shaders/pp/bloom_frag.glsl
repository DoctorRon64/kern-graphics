#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D image;

void main()
{            
    //FragColor = vec4(FragColor.rgb, 1.0);
    //FragColor = texture(image, fragCoord);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        FragColor = vec4(FragColor.rgb, 1.0);
    else
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}