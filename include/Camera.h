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
	glm::vec3 Position;
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Orientation = glm::vec3(0.3f, -0.3f, -1.0f); // orientation is reversed
	//glm::vec3 Orientation = glm::vec3(0.0f,-0.5f,0.0f); // orientation is reversed

	std::string name;

	glm::mat4 Projection;

	glm::mat4 cameraMatrix = glm::mat4(1.0f); // projection * view
	int width, height;
	float fov, near, far;
	float speed = 0.001f, sensitivity = 100.0f;

	double posX, posY;
	double previousX, previousY;
	bool firstClick = true;
public:

	Camera(int width, int height, glm::vec3 Position,std::string name);
	void Update();
	void CameraUniform(Shader& shader, const char* uniform);

	void setOrientation(glm::vec3 ori);

	void setPerspectiveProjection(float fovy, float aspect, float near, float far);
	void setOrthographicProjection();
	void setProjection(glm::mat4 projection) { this->Projection = projection; }

	glm::mat4  getViewMatrix()const;
	glm::mat4  getProjectionMatrix()const;
	std::string getName() const { return name; }

	int getWidth()const;
	int getHeight()const;
	glm::vec3 getPosition() const;
	glm::vec3 getOrientation()const;
	float getFOV()const;

	void setWidth(int width);
	void setHeight(int height);

	void setFOV(float fov);
	void setPosition(glm::vec3 position);

	Ray CreateRay(GLFWwindow* window);

	void Movement3D(GLFWwindow* glfwWindow, MyGUI& gui);
	void Movement2D(GLFWwindow* glfwWindow, MyGUI& gui);


	void setCamera2D();

};
