#pragma once

#include "glad/glad.h"
#include "DVertex.h"
#include <vector>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

class DrawableAABB {
	VAO VAO;
	std::vector<DVertex>vertices;
	std::vector<GLuint>indices;

public:

	DrawableAABB() {};
	DrawableAABB(glm::vec3 min, glm::vec3 max);
	DrawableAABB(const DrawableAABB& a);
	DrawableAABB operator=(const DrawableAABB& a);

	void Draw(Camera& camera, Shader& shader);
};
