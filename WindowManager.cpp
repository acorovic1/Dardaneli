#include "WindowManager.h"


WindowManager* WindowManager::instancePtr = nullptr;

WindowManager* windowSingleton = WindowManager::getInstance();

WindowManager* WindowManager::getInstance() {
	if (!instancePtr)
		instancePtr = new WindowManager();
	return instancePtr;
}



void WindowManager::addWindow(Window* object)
{
	windows.push_back(object);

}

void WindowManager::deleteWindow(GLuint index)
{
	windows.erase(windows.begin() + index);

};

Window* WindowManager::getWindow(std::string name)
{
	for (auto* x : windows)
		if (x->getName() == name)
			return x;

	std::cout << "\nINVALID NAME ... getWindow() function\n";
	return nullptr;
}

