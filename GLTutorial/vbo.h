#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>


class VBO {
private:
	unsigned int _id;
public:
	VBO(float* data);
};