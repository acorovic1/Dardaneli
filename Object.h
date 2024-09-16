#pragma once

#define OBJECT_CLASS_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

#include "ObjectManager.h"

#include <vector>


class Object {
	GLuint index{};


public:
	Object();

	GLuint getIndex()const;

	virtual void Draw(Shader& shader, Camera& camera) = 0;

	virtual void Translate(glm::vec3& translateVector) =0;
	virtual void Translate(float x, float y, float z) =0;

	virtual void Rotate(float degrees, glm::vec3& axisVector) =0;

	virtual void Scale(glm::vec3& scaleVector) =0;
	virtual void Scale(float x, float y, float z) =0;
	
};
	