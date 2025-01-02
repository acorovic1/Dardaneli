#pragma once


#include "Object.h"
#include "algorithm"
#include "Window.h"


class Renderer {


	
public:

	Renderer();

	void Init();
	void Render(Window& window,MyGUI &gui);




};