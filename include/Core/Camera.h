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
class Window;

class Camera {
	std::string name;

	glm::vec3 position;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 orientation = glm::vec3(0.3f, -0.3f, -1.0f); // orientation is reversed

	int width, height;
	float fov, near, far; // field of view, near plane, far plane
	float speed = 0.001f, sensitivity = 100.0f;

	glm::mat4 projection;
	glm::mat4 cameraMatrix = glm::mat4(1.0f); // projection * view


public:

	Camera(int width, int height, glm::vec3 Position, std::string name);
	void update();
	void cameraUniform(bool activated, Shader& shader, const char* uniform);



	glm::mat4  getViewMatrix()const { return glm::lookAt(position, position + orientation, up); };
	glm::mat4  getProjectionMatrix()const { return projection; }
	std::string getName() const { return name; }
	int getWidth()const { return width; }
	int getHeight()const { return height; }
	glm::vec3 getPosition() const { return position; };
	glm::vec3 getOrientation()const { return orientation; };

	glm::vec3 getUp()const { return up; };
	float getFOV()const {  return fov; };


	void setWidth(int width) { Camera::width = width; };
	void setHeight(int height) { Camera::height = height; };
	void setFOV(float fov) { setPerspectiveProjection(fov, float(getWidth()) / float(getHeight()), near, far); }
	void setPosition(glm::vec3 position) { Camera::position = position; };
	void setOrientation(glm::vec3 ori) { Camera::orientation = ori; };
	void setProjection(glm::mat4 projection) { Camera::projection = projection; };

	void setPerspectiveProjection(float fovy, float aspect, float near, float far);
	void setOrthographicProjection();

	Ray createRay(GLFWwindow* window);

	void movement3D(GLFWwindow* glfwWindow);
	void movement2D(GLFWwindow* glfwWindow);


	void setCamera2D();

};
