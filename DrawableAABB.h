#pragma once

#include "glad/glad.h"
#include "Vertex.h"
#include <vector>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"



class DrawableAABB {

	VAO VAO;
	std::vector<Vertex>vertices;
	std::vector<GLuint>indices;

public :
	DrawableAABB() {};
	DrawableAABB(glm::vec3 min, glm::vec3 max);

	void Draw(Camera& camera,Shader& shader);
};
