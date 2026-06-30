#pragma once

#include "Object.h"
#include "algorithm"
#include "Window.h"


class Renderer {
	Window& window;
	MyGUI& gui;



public:

	Renderer(Window& window, MyGUI& gui);

	void init();
	void render();


private:
	void viewportEditor();
	void uvEditor();
	void shaderEditor();
	

};