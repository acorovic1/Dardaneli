#pragma once



#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/detail/_swizzle.hpp>


#include "Shader.h"
#include "Ray.h"

class MyGUI;

class Camera {


	glm::vec3 Position;
	glm::vec3 Up = glm::vec3(0.0f,1.0f,0.0f);
	glm::vec3 Orientation = glm::vec3(0.3f,-0.3f,-1.0f); // orientation is reversed

	glm::mat4 Projection;

	glm::mat4 cameraMatrix = glm::mat4(1.0f); // projection * view
	int width, height;
	float speed =0.001f, sensitivity = 100.0f;

	bool firstClick = true;
public:

	Camera(int width, int height, glm::vec3 Position);
	void Update();
	void CameraUniform(Shader& shader, const char* uniform);

	void setProjectionMatrix(glm::mat4& projection);
	void setProjectionMatrix(glm::mat4 projection);

	glm::mat4  getViewMatrix()const;
	glm::mat4  getProjectionMatrix()const;

	int getWidth()const;
	int getHeight()const;
	glm::vec3 getPosition() const;

	void setWidth(int width);
	void setHeight(int height);

		


	Ray CreateRay(GLFWwindow* window);

	void Inputs(GLFWwindow* window,MyGUI &gui);
};

