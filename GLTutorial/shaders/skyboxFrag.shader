#version 330 core
out vec4 FragColor;

in vec3 textureDir;

uniform samplerCube texture_skybox1;

void main() {
	FragColor = texture(texture_skybox1, textureDir);
}