#version 330 core

in vec2 texCoord;

out vec4 FragColor;
uniform vec3 lightColor;
uniform sampler2D texture_diffuse;

void main()
{
    FragColor = vec4(vec3(texture(texture_diffuse, texCoord)), 1.0);
}