#pragma once
#include "Window.h"

class WindowManager {

	std::vector<Window*> windows;
	static WindowManager* instancePtr;

	WindowManager() :windows(0) {}



public:

	static WindowManager* getInstance();

	WindowManager(const WindowManager& copy) = delete;
	void operator=(const WindowManager& copy) = delete;

	void addWindow(Window* window);
	void deleteWindow(GLuint index);

	Window* getWindow(std::string name);



};


extern WindowManager* windowSingleton;