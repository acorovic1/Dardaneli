#pragma once
#include "glm/common.hpp"

struct UVVertex {

	glm::vec2 uv;


	UVVertex() : uv(0.0f, 0.0f) {}
	explicit UVVertex(glm::vec2 uvPos) : uv(uvPos) {}

	UVVertex(const UVVertex& copy) { this->uv = copy.uv; };
	UVVertex& operator=(const UVVertex& copy) {this->uv = copy.uv;};

	UVVertex(UVVertex&&) = delete;
	UVVertex& operator=(UVVertex&&) = delete;


};