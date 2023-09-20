#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>


class VAO {
private:
	unsigned int _id;
	unsigned int _bufferId;

public:
	VAO(const std::vector<float>& data, const std::vector<int>& attribs, int stride);
	unsigned int getId();
	unsigned int getBufferId();
};