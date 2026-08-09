#pragma once

#include "Object.h"
#include "algorithm"
#include "Window.h"


class Renderer {
	Window& window;
	//MyGUI& gui;



public:

	Renderer(Window& window);

	void init();
	void render();


private:
	void objectEditor(Viewport* viewport);
	void uvEditor(Viewport* viewport);
	void shaderEditor(Viewport* viewport);
	

};