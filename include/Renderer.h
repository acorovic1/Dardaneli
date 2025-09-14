#pragma once

#include "Object.h"
#include "algorithm"
#include "Window.h"


class Renderer {
	Window& window;
	MyGUI& gui;

	void viewportEditor();
	void uvEditor();
	void shaderEditor();
	


public:

	Renderer(Window& window, MyGUI& gui);

	void Init();
	void Render();



	

};