#version 330 core
#define MAX_LIGHTS 128

out vec4 FragColor;

in vec3 FragPos;
in vec4 FragPosLightSpace;

in vec3 Normal;
in vec2 TexCoords;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLights[MAX_LIGHTS];
uniform DirectionalLight sun;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_bump1;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform sampler2D shadow_map;
uniform Material material;
uniform int numPointLights;

uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
float CalcShadow(vec4 fragPosLightSpace, float bias);
void gamma_correct(inout vec3 diffuse);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    
    //for(int i = 0; i < numPointLights; i++) {
    //    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    //}
    

    result += CalcDirectionalLight(sun, norm, viewDir);

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float  spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);

    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * material.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

    // gamma correct diffuse
    gamma_correct(diffuse);

    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) 
{
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(light.direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 color = texture(material.texture_diffuse1, TexCoords).rgb;

    // combine results
    vec3 ambient = light.ambient * material.ambient * color;
    vec3 diffuse = light.diffuse * material.diffuse * diff * color;

    // gamma correct diffuse
    gamma_correct(diffuse);

    vec3 specular = light.specular * material.specular * spec * color;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = CalcShadow(FragPosLightSpace, bias);   
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));  

    return lighting;
}

void gamma_correct(inout vec3 diffuse)
{
    float gamma = 2.2f;
    diffuse = pow(diffuse, vec3(1.0 / gamma));
}

float CalcShadow(vec4 fragPosLightSpace, float bias) 
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 

    float closestDepth = texture(shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;

    if(currentDepth > 1.0)
        return 0.0;

    return (currentDepth - bias < closestDepth) ? 0.0 : 1.0;
}