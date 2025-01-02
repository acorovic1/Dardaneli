#pragma once



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

class Window {
	GLFWwindow* window;
	Camera camera;

public:
	Window(int width, int height, const char* title);
	void Init();
	void Terminate();
	bool ShouldClose();
	void PollEvents();
	GLFWwindow* GetWindow();

	Camera& getCamera();
	void resizeWindow(int width,int height);
};