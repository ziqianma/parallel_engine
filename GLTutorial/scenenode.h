#pragma once
#include "common.h"
#include "mesh.h"

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class SceneNode {
public:
	SceneNode(Mesh* mesh);
	~SceneNode();
	void addChild(SceneNode* child);		
private:
	SceneNode* parent;
	std::vector<SceneNode*> children;
};