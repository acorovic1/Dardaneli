#pragma once

#include "Object.h"
#include "algorithm"
#include "Window.h"


class Renderer {
	Window& window;
	MyGUI& gui;
public:

	Renderer(Window& window, MyGUI& gui);

	void Init();
	void Render();



	void RenderUVEditor();

};