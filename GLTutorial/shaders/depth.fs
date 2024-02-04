#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float nearPlane;
uniform float farPlane;

float LinearizeDepth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depthValue = texture(depthMap, TexCoords).r;
    float linearDepthVal = LinearizeDepth(depthValue, nearPlane, farPlane) / farPlane;

    FragColor = vec4(vec3(depthValue), 1.0);
}   