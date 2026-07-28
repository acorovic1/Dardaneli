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
#include "Viewport.h"

// kada budem pravio vise window-a \\

	// modove razdvojiti, shader editor i uv editor ne trebaju spadati medju modove, vec da imaju svoj EDITOR TYPE
	// modovi trebaju biti object,edit,sculpt i slicno

	// varijable kao sto su renderMode iz Application-a prebaciti u Window (ili MyGUI)



// kada budem pravio vise window-a \\


class Application;
class Window {

	GLFWwindow* glfwWindow;
	std::unique_ptr<MyGUI> gui;

	std::string name;

	int width, height;

	std::vector<std::unique_ptr<Viewport>> viewports;

	double posX = 0, posY = 0;
	double previousX = 0, previousY = 0;
	bool firstClick = true;


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


	GLFWwindow* getGLFWwindow()const { return glfwWindow; }
	MyGUI& getGui() const { return *gui.get(); }
	std::vector<std::unique_ptr<Viewport>>& getViewports() { return viewports; }

	std::string getName()const { return name; };
	int getWidth() const { return width; };
	int getHeight()const { return height; };

	double& getPosX() { return posX; }
	double& getPreviousX() { return previousX; }
	double& getPosY() { return posY; }
	double& getPreviousY() { return previousY; }
	bool& getFirstClick() { return firstClick; }

	void addViewport(Viewport* baseViewport, double xPos, double yPos, bool vertical);
	//Viewport* getViewportClosestToCursor();



	void resizeWindow(int width, int height);
	void splitWindow(int width, int height);


	Viewport* getViewportAtCursor(double x=-1,double y=-1);
	void deleteViewport(Viewport* viewport);
	void deleteDegenerateViewports();


};
