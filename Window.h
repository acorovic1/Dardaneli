#pragma once



#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Camera.h"
#include "CameraManager.h"
#include "Ray.h"
#include "ObjectManager.h"
#include "ObjectModeBVH.h"



class Application;
class Window {
	GLFWwindow* window;
	Camera camera;

	std::string name;

	double posX=0, posY=0;
	double previousX=0, previousY=0;
	bool firstClick = true;

	std::vector<int>keys = std::vector<int>(1024, 0);      // Holds current state (pressed or not)
	std::vector<int>keysProcessed = std::vector<int>(1024, 0);    // Ensures action happens once per press
	std::vector<int>mouseButtons = std::vector<int>(2, 0);
	std::vector<int>mouseButtonsProcessed = std::vector<int>(2, 0);


public:
	Window(int width, int height, const char* title);
	void Init();
	void Terminate();
	bool ShouldClose();
	void PollEvents();
	GLFWwindow* GetWindow(); //promijeni veliko G u malo g... konzistentnost
	std::string getName() { return name; };

	Camera& getCamera();
	void resizeWindow(int width,int height);
	
	void setCallbacks();
	static void key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	std::vector<int>& getKeys() { return keys; }
	std::vector<int>& getKeysProcessed() { return keysProcessed; }
	std::vector<int>& getMouseButtons() { return mouseButtons; }
	std::vector<int>& getMouseButtonsProcessed() { return mouseButtonsProcessed; }

	double& getPosX() { return posX; }
	double& getPreviousX() { return previousX; }
	double& getPosY() { return posY; }
	double& getPreviousY() { return previousY; }
	bool& getFirstClick() { return firstClick; }




	

};

