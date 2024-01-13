#version 330 core
out vec4 FragColor;

in vec3 textureDir;

uniform samplerCube skyboxTexture;

void main() {
	FragColor = texture(skyboxTexture, textureDir);
}