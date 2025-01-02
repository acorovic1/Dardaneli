#pragma once



#include "glm/glm.hpp"
#include <glad/glad.h>

#include "Ray.h"
#include "Camera.h"
#include "Shader.h"
#include "VAO.h"
#include "Object.h"
#include "DrawableAABB.h"


inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }


struct AABB {

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	DrawableAABB aabb;

	AABB();
	AABB(glm::vec3 min, glm::vec3 max);
	AABB(AABB& a, AABB& b);
	AABB(Object& mesh);
	AABB(glm::vec3& vertex);

	bool intersectRayAABB(const Ray& ray);

	void Draw( Camera& camera,Shader &shader);

};