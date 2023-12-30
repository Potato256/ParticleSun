#version 330 core

struct Material {
    float shininess;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D texture1;
}; 

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 7

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int lightMode;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0f); 
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
    if(lightMode == 0) 
        FragColor = vec4(result.xyz * vec3(texture(material.texture1, TexCoords) ) , 1.0f); 
    else if(lightMode == 1) 
        FragColor = vec4(result.yxz * vec3(texture(material.texture1, TexCoords) ) , 1.0f); 
    else if(lightMode == 2) 
        FragColor = vec4(result.zyx * vec3(texture(material.texture1, TexCoords) ) , 1.0f); 
    else if(lightMode == 3) 
        FragColor = vec4(result.xzy * vec3(texture(material.texture1, TexCoords) ) , 1.0f); 
    else if(lightMode == 4) 
        FragColor = vec4(result.yzx * vec3(texture(material.texture1, TexCoords) ) , 1.0f); 
    else if(lightMode == 5) 
        FragColor = vec4(result.zxy * vec3(texture(material.texture1, TexCoords) ) , 1.0f); 
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = material.ambient * attenuation;
    vec3 diffuse = material.diffuse * diff * attenuation;
    vec3 specular = material.specular * spec * attenuation;

    return ambient + light.color * (diffuse + specular);
}