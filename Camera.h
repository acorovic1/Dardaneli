#pragma once

#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


#include "Shader.h"

class Camera {


public:
	glm::vec3 Position;
	glm::vec3 Up = glm::vec3(0.0f,1.0f,0.0f);
	glm::vec3 Orientation = glm::vec3(0.0f,0.0f,-1.0f); // direction is reversed

	glm::mat4 cameraMatrix = glm::mat4(1.0f); // projection * view
	int width, height;
	float speed =0.1f, sensitivity = 100.0f;

	bool firstClick = true;

	Camera(int width, int height, glm::vec3 Position);
	void Update(float FOVangle, float nearPlane, float farPlane);
	void CameraUniform(Shader& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
};

