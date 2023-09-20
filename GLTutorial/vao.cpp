#include "vao.h"

VAO::VAO(const std::vector<float>& data, const std::vector<int>& attribs, int stride) {
	glGenVertexArrays(1, &_id);
	glGenBuffers(1, &_bufferId);

	glBindVertexArray(_id);

	glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

	// bind attributes
	int totalSize = 0;
	for (int i = 0; i < attribs.size(); i++) {
		glVertexAttribPointer(i, attribs[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(totalSize * sizeof(float)));
		glEnableVertexAttribArray(i);
		totalSize += attribs[i];
	}
}
		
unsigned int VAO::getId() {
	return _id;
}

unsigned int VAO::getBufferId() {
	return _bufferId;
}