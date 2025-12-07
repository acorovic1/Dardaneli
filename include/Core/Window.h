#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Camera.h"
#include "CameraManager.h"
#include "Ray.h"
#include "ObjectManager.h"
#include "ObjectModeBVH.h"
#include "MyGUI.h"

// kada budem pravio vise window-a \\

	// modove razdvojiti, shader editor i uv editor ne trebaju spadati medju modove, vec da imaju svoj EDITOR TYPE
	// modovi trebaju biti object,edit,sculpt i slicno
	
	// varijable kao sto su renderMode iz Application-a prebaciti u Window (ili MyGUI)
	


// kada budem pravio vise window-a \\


class Application;
class Window {
	/*GLFWwindow* window;*/
	Camera* camera;
	MyGUI* gui;


	std::string name;

	int width, height;

	double posX = 0, posY = 0;
	double previousX = 0, previousY = 0;
	bool firstClick = true;

	std::vector<int>keys = std::vector<int>(1024, 0);      // Holds current state (pressed or not)
	std::vector<int>keysProcessed = std::vector<int>(1024, 0);    // Ensures action happens once per press
	std::vector<int>mouseButtons = std::vector<int>(3, 0);
	std::vector<int>mouseButtonsProcessed = std::vector<int>(3, 0);

public:
	Window(const char* title);
	void init();
	void terminate();
	bool shouldClose();
	void pollEvents();


	static void key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void setCallbacks();


	MyGUI& getGui() { return *gui; }
	Camera* getCamera() { return camera; }
	GLFWwindow* getGLFWwindow() { return gui->getGLFWwindow(); }

	std::string getName() { return name; };
	int getWidth() { return width; };
	int getHeight() { return height; };

	std::vector<int>& getKeys() { return keys; }
	std::vector<int>& getKeysProcessed() { return keysProcessed; }
	std::vector<int>& getMouseButtons() { return mouseButtons; }
	std::vector<int>& getMouseButtonsProcessed() { return mouseButtonsProcessed; }

	double& getPosX() { return posX; }
	double& getPreviousX() { return previousX; }
	double& getPosY() { return posY; }
	double& getPreviousY() { return previousY; }
	bool& getFirstClick() { return firstClick; }


	void setCamera(Camera* cam);
	void resizeWindow(int width, int height);
	void splitWindow(int width, int height);


};
