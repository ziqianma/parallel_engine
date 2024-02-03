#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    float red = texture(screenTexture, TexCoords).r;
    FragColor = vec4(red, 0.0, 0.0, 1.0);
}   