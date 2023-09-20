#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;

uniform vec4 lightColor;
uniform vec4 objectColor;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), lightColor * objectColor, 0.5);
}