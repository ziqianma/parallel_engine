#include "framebuffer.h"

FrameBuffer::FrameBuffer(std::unique_ptr<RenderBuffer> buffer, unsigned int width, unsigned int height, GLenum attachment, GLenum textureFormat, const float quadVertices[]) :
	m_FrameBufferWidth(width),
	m_FrameBufferHeight(height),
	m_FrameBufferTextureID(0),
	m_QuadVAO(0),
	m_QuadVBO(0),
	m_RenderBuffer(nullptr)
{
	glGenFramebuffers(1, &m_FrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObject);

	// create texture attachment
	glGenTextures(1, &m_FrameBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_FrameBufferTextureID, 0);

	if (buffer) {
		buffer->attach();
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Failed to bind framebuffer! id: " << m_FrameBufferObject << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_RenderBuffer = std::move(buffer);
	setup_quad(quadVertices);
}

void FrameBuffer::setup_quad(const float quadVertices[]) {
	glGenVertexArrays(1, &m_QuadVAO);
	glGenBuffers(1, &m_QuadVBO);

	glBindVertexArray(m_QuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

void FrameBuffer::draw_onto_quad() const {
	glBindVertexArray(m_QuadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

void FrameBuffer::bind() const {	
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObject);
}

void FrameBuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderBuffer::RenderBuffer(unsigned int width, unsigned int height, GLenum attachment, GLenum storage) : m_Attachment(attachment) {
	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, storage, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::attach() const {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, m_Attachment, GL_RENDERBUFFER, m_RBO); // now actually attach it
}

void RenderBuffer::unbind() const {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}