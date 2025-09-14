#include "Window.h"

Window::Window(const char* title)
	: name(title), camera(nullptr)
{

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	window = glfwCreateWindow(1, 1, title, nullptr, nullptr);
	glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

	glfwGetWindowSize( window, &width, &height);

}

void Window::Init() {
	glfwMakeContextCurrent(window);
	gladLoadGL();
	//glViewport(0, 0, camera.getWidth(), camera.getHeight());

	setCallbacks();
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::Terminate()
{
	glfwDestroyWindow(window);
}

void Window::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window) { std::cout << "NE VALJA PROYOR"; return; }
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			window->keys[key] = (window->keys[key] + 1) % 3;
		}
		else if (action == GLFW_RELEASE) {
			// window->keys[key] = 0;
			// window->keysProcessed[key] = false;  // Reset when released
		}
	}

	//std::cout << "\n Keys pressed: ";
	//for (int i = 0; i < 1024; i++)
	//    if (window->keys[i])
	//        std::cout <<"Value " << window->keys[i] << " Key number " << i << " ";
	//std::cout << "\n ----- ";
}
void Window::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
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
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//scroll down = -1 ... scroll up = +1
	Window* classWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	Camera* camera = classWindow->getCamera();

	//std::cout << "\n\t" << camera->getName() << " ";

	glm::vec3 offset = camera->getPosition() + (static_cast<float>(yoffset) / 2) * glm::normalize(camera->getOrientation());

	camera->setPosition(offset);

	//std::cout << camera->getPosition().z;
}

void Window::setCallbacks()
{
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void Window::PollEvents() {
	static 	int width, height, widthPrev, heightPrev;

	glfwGetWindowSize(window, &width, &height);
	if (width != widthPrev || height != heightPrev)
	{
		resizeWindow(width, height);
		widthPrev = width;
		heightPrev = height;

		std::cout << "Window resized -- Width: " << width << " Height: " << height << "\n";
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

GLFWwindow* Window::getWindow() {
	return window;
}

Camera* Window::getCamera()
{
	return camera;
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
		glfwIconifyWindow(window);
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



//Camera& Window::getCamera() { return camera; }