#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform vec4 ourColor;
uniform sampler2D texture1;

void main()
{
    FragColor = ourColor * texture(texture1, TexCoord);
}