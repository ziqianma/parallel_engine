#pragma once
#include "common.h"
#include "shader/shader.h"
#include "textures.h"

class RenderBuffer;
class FrameBuffer {
public:
	FrameBuffer(const Shader& shader, std::unique_ptr<RenderBuffer> buffer, const std::string& textureName, unsigned int width, unsigned int height, GLenum attachment, GLenum internalFormat, GLenum format, GLenum dataType, const float quadVertices[]);
	void bind() const;
	void unbind() const;

	void draw_onto_quad() const;

	unsigned int getTextureID() const { 
		return m_FrameBufferTextureID;
	}
private:
	std::unique_ptr<RenderBuffer> m_RenderBuffer;
	unsigned int m_FrameBufferWidth, m_FrameBufferHeight;
	
	unsigned int m_FrameBufferTextureID;
	unsigned int m_FrameBufferTextureUnit;

	unsigned int m_FrameBufferObject;

	unsigned int m_QuadVAO;
	unsigned int m_QuadVBO;

	void setup_quad(const float quadVertices[]);
};

class RenderBuffer {
public:
	RenderBuffer(unsigned int width, unsigned int height, GLenum attachment, GLenum storage);

	void attach() const;
	void unbind() const;

private:
	unsigned int m_RBO;
	unsigned int m_Attachment;
};