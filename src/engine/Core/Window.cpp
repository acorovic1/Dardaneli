#include "Window.h"
#include "MyGUI.h"

Window::Window(const char* title)
	: name(title), glfwWindow(nullptr)
{

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	glfwWindow = glfwCreateWindow(1, 1, title, nullptr, nullptr);
	glfwMakeContextCurrent(glfwWindow);
	gladLoadGL();



	glfwSetWindowUserPointer(glfwWindow, reinterpret_cast<void*>(this));
	glfwGetWindowSize(glfwWindow, &width, &height);

	viewports.emplace_back(std::make_unique<Viewport>(0.0, 0.0, 1.0, 1.0, glfwWindow));

	gui = std::make_unique<MyGUI>(glfwWindow);

}

void Window::init() { setCallbacks(); }

void Window::terminate() {
	gui->shutdown();
	glfwDestroyWindow(glfwWindow);
}

bool Window::shouldClose() { return glfwWindowShouldClose(glfwWindow); }
void Window::pollEvents() {
	static 	int width, height, widthPrev, heightPrev;

	glfwGetWindowSize(glfwWindow, &width, &height);
	if (width != widthPrev || height != heightPrev)
	{
		resizeWindow(width, height);
		widthPrev = width;
		heightPrev = height;

		//std::cout << "Window resized -- Width: " << width << " Height: " << height << "\n";
	}

	glfwSwapBuffers(glfwWindow);
	glfwPollEvents();
}

void Window::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(glfwWindow, key, scancode, action, mods);

	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	Viewport* viewport = window->getViewportAtCursor();
	if (!viewport)return;
	std::vector<int>& keys = viewport->getKeys();
	if (key >= 0 && key < 1024)
		if (action == GLFW_PRESS)
			keys[key] = (keys[key] + 1) % 3;


}
void Window::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(glfwWindow, button, action, mods);
	if (button > 2)return;
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	Viewport* viewport = window->getViewportAtCursor();
	if (!viewport)return;
	std::vector<int>& mouseButtons = viewport->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = viewport->getMouseButtonsProcessed();
	if (action == GLFW_PRESS)
	{
		mouseButtons[button] = 1;
		mouseButtonsProcessed[button] = 0;
	}
	else if (action == GLFW_RELEASE)
	{
		mouseButtons[button] = 0;
		mouseButtonsProcessed[button] = 1;
	}
}
void Window::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {

	ImGui_ImplGlfw_ScrollCallback(glfwWindow, xoffset, yoffset);

	//scroll down = -1 ... scroll up = +1
	Window* classWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	Viewport* viewport = classWindow->getViewportAtCursor();
	if (!viewport)return;
	Camera* camera = viewport->getActiveCamera();

	//std::cout << "\n\t" << camera->getName() << " ";

	glm::vec3 offset = camera->getPosition() + (static_cast<float>(yoffset) / 2) * glm::normalize(camera->getOrientation());

	camera->setPosition(offset);

	//std::cout << camera->getPosition().z;
}

void Window::setCallbacks()
{
	glfwSetKeyCallback(glfwWindow, key_callback);
	glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
	glfwSetScrollCallback(glfwWindow, scroll_callback);
}


void Window::addViewport(Viewport* baseViewport, double xPos, double yPos, bool vertical)
{
	viewports.emplace_back(std::make_unique<Viewport>(baseViewport, xPos, yPos, vertical, this));
}

//Viewport* Window::getViewportClosestToCursor()
//{
//	Viewport* returnViewport = viewports[0].get();
//	// ne radi sa normalizovanim koordinatama viewporta
//	int tempX = posX;
//	if (tempX > width) tempX = width;
//	else if (tempX < 0) tempX = 0;
//
//	int tempY = posY;
//	if (tempY > height) tempY = height;
//	else if (tempY < 0) tempY = 0;
//
//
//	for (int i = 1;i < viewports.size();i++)
//		if (tempX >= viewports[i]->getLeft() && tempX <= viewports[i]->getRight() &&
//			tempY >= viewports[i]->getBottom() && tempY <= viewports[i]->getTop())
//			return viewports[i].get();
//
//	std::cout << "\n\n\tNo viewport found at cursor position (" << posX << ", " << posY << ") !!!\n\n";
//	return nullptr;
//}

void Window::resizeWindow(int width, int height)
{
	if (width == 0 || height == 0)
		glfwIconifyWindow(glfwWindow);
	else
	{
		for (auto& viewport : viewports)
			viewport->resizeWindow(width, height);

		this->width = width;
		this->height = height;
	}
}

void Window::splitWindow(int width, int height)
{

}

Viewport* Window::getViewportAtCursor(double x, double y)
{
	if (x == -1 && y == -1)
	{
		glfwGetCursorPos(glfwWindow, &x, &y);


	}
	//std::cout << "\n\t Cursor position " << x << " " << y;
	y = height - y;


	for (int i = 0;i < viewports.size();i++)
	{
		glm::ivec4 corners = viewports[i]->getCorners(this);
		if (x >= corners.x && x <= corners.z &&
			y >= corners.y && y <= corners.w)
		{
			//std::cout << "\n\n\t\t RETURNED VIEWPORT " << i << "with corners at " << corners.x << " " << corners.y << " " << corners.z << " " << corners.w;
			return viewports[i].get();
		}
	}

	//std::cout << "\n\n\tNo viewport found at cursor position (" << cursorX << ", " << cursorY << ") !!!\n\n";
	return nullptr;

}

void Window::deleteViewport(Viewport* viewport)
{

	auto it = std::find_if(viewports.begin(), viewports.end(),
		[viewport](const std::unique_ptr<Viewport>& vp)
		{
			return vp.get() == viewport;
		});

	if (it != viewports.end())
	{
		viewports.erase(it);
	}
}

void Window::deleteDegenerateViewports()
{
	for (int i = 0;i < viewports.size();i++)
		if (viewports[i]->getTop() - viewports[i]->getBottom() < myEpsilon || viewports[i]->getRight() - viewports[i]->getLeft() < myEpsilon)
		{
			std::cout << "\n\t Deleted a degenerate viewport!";
			
			viewports.erase(viewports.begin() + i);
		}


}


