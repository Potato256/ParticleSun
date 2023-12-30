#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture1;
uniform float alpha;
uniform float mix_k;
uniform int lightMode;
void main()
{
    vec4 t = texture(texture1, TexCoords);
    vec3 acolor = mix(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.5f, 0.0f), mix_k);
    
    if (lightMode == 0)
        FragColor = vec4(acolor.xyz * t.xyz , alpha * t.w); 
    else if (lightMode == 1)
        FragColor = vec4(acolor.yxz * t.xyz , alpha * t.w); 
    else if (lightMode == 2)
        FragColor = vec4(acolor.zyx * t.xyz , alpha * t.w); 
    else if (lightMode == 3)
        FragColor = vec4(acolor.xzy * t.xyz , alpha * t.w); 
    else if (lightMode == 4)
        FragColor = vec4(acolor.yzx * t.xyz , alpha * t.w); 
    else if (lightMode == 5)
        FragColor = vec4(acolor.zxy * t.xyz , alpha * t.w); 
}