#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D texture1;
uniform int lightMode;
void main()
{
    if(lightMode == 0) 
        FragColor = vec4(vec3(texture(texture1, TexCoords)).xyz, 1.0f); 
    else if(lightMode == 1) 
        FragColor = vec4(vec3(texture(texture1, TexCoords)).yxz, 1.0f); 
    else if(lightMode == 2) 
        FragColor = vec4(vec3(texture(texture1, TexCoords)).zyx, 1.0f); 
    else if(lightMode == 3) 
        FragColor = vec4(vec3(texture(texture1, TexCoords)).xzy, 1.0f); 
    else if(lightMode == 4) 
        FragColor = vec4(vec3(texture(texture1, TexCoords)).yzx, 1.0f); 
    else if(lightMode == 5) 
        FragColor = vec4(vec3(texture(texture1, TexCoords)).zxy, 1.0f); 
}