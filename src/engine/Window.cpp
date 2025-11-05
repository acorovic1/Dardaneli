#include "Window.h"
#include "MyGUI.h"

Window::Window(const char* title)
	: name(title), camera(nullptr), gui(nullptr)
{

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, title, nullptr, nullptr);
	glfwMakeContextCurrent(glfwWindow);
	gladLoadGL();

	glfwSetWindowUserPointer(glfwWindow, reinterpret_cast<void*>(this));
	glfwGetWindowSize(glfwWindow, &width, &height);

	gui = new MyGUI(glfwWindow);

}

void Window::init() {

	setCallbacks();
}

void Window::terminate()
{
	gui->shutdown();
	glfwDestroyWindow(gui->getGLFWwindow());

	delete gui;
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(gui->getGLFWwindow());
}

void Window::pollEvents() {
	static 	int width, height, widthPrev, heightPrev;

	glfwGetWindowSize(gui->getGLFWwindow(), &width, &height);
	if (width != widthPrev || height != heightPrev)
	{
		resizeWindow(width, height);
		widthPrev = width;
		heightPrev = height;

		std::cout << "Window resized -- Width: " << width << " Height: " << height << "\n";
	}

	glfwSwapBuffers(gui->getGLFWwindow());
	glfwPollEvents();
}

void Window::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(glfwWindow, key, scancode, action, mods);

	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	if (key >= 0 && key < 1024)
		if (action == GLFW_PRESS)
			window->keys[key] = (window->keys[key] + 1) % 3;


}
void Window::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(glfwWindow, button, action, mods);
	if (button > 2)return;
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	if (action == GLFW_PRESS)
	{
		window->mouseButtons[button] = 1;
		window->mouseButtonsProcessed[button] = 0;
	}
	else if (action == GLFW_RELEASE)
	{
		window->mouseButtons[button] = 0;
		window->mouseButtonsProcessed[button] = 1;
	}
}
void Window::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {

	ImGui_ImplGlfw_ScrollCallback(glfwWindow, xoffset, yoffset);

	//scroll down = -1 ... scroll up = +1
	Window* classWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	Camera* camera = classWindow->getCamera();

	//std::cout << "\n\t" << camera->getName() << " ";

	glm::vec3 offset = camera->getPosition() + (static_cast<float>(yoffset) / 2) * glm::normalize(camera->getOrientation());

	camera->setPosition(offset);

	//std::cout << camera->getPosition().z;
}

void Window::setCallbacks()
{
	glfwSetKeyCallback(gui->getGLFWwindow(), key_callback);
	glfwSetMouseButtonCallback(gui->getGLFWwindow(), mouse_button_callback);
	glfwSetScrollCallback(gui->getGLFWwindow(), scroll_callback);
}


void Window::setCamera(Camera* cam)
{
	camera = cam;
	std::cout << "\n\nCamera set to: " << camera->getName() << "\n";
	camera->setWidth(width);
	camera->setHeight(height);
	if (camera->getName() == "Viewport")
		camera->setPerspectiveProjection(camera->getFOV(), float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 100.0f);
}

void Window::resizeWindow(int width, int height)
{
	if (width == 0 || height == 0)
		glfwIconifyWindow(gui->getGLFWwindow());
	else
	{
		this->width = width;
		this->height = height;

		float aspect = float(width) / float(height);

		camera->setWidth(width);
		camera->setHeight(height);
		if (camera->getName() == "Viewport")
			camera->setPerspectiveProjection(45, float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 100.0f);
		if (camera->getName() == "UV")
			camera->setProjection(glm::ortho(aspect * -1.0f, aspect * 1.0f, -1.0f, 1.0f, -2000.0f, 300000.0f));
	}
	glViewport(0, 0, width, height);
}

void Window::splitWindow(int width, int height)
{
	glViewport(0, 0, width / 2, height);
}


