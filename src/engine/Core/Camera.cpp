#include "Camera.h"
#include "MyGUI.h"
#include "CameraManager.h"
#include "Window.h"
#include "Viewport.h"

Camera::Camera(int width, int height, glm::vec3 position, std::string name)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
	Camera::name = name;

	fov = 45.0, near = 0.01f, far = 100.0f;

	//cameraSingleton->addCamera(this);
}

Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up, std::string name)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
	Camera::orientation = orientation;
	Camera::up = up;
	Camera::name = name;
	fov = 45.0, near = 0.01f, far = 100.0f;
	//cameraSingleton->addCamera(this);
}

Camera::Camera(const Camera& copy)
{
	width = copy.width;
	height = copy.height;
	position = copy.position;

	fov = copy.fov;
	near = copy.near;
	far = copy.far;

	projection = copy.projection;
	cameraMatrix = copy.cameraMatrix;

	// ovo je covek neki sto mi uzeee sve
	name = copy.name;
	//cameraSingleton->addCamera(this);
}

void Camera::update()
{
	cameraMatrix = projection * getViewMatrix(); // projection * view
}
void Camera::cameraUniform(bool activated, Shader& shader, const char* uniform) {
	if (!activated)
		shader.activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::setPerspectiveProjection(float fov, float aspect, float near, float far)
{
	Camera::fov = fov;
	Camera::near = near;
	Camera::far = far;
	this->projection = glm::perspective(glm::radians(fov), aspect, near, far);
	this->update();
}

void Camera::setOrthographicProjection(float scale, float aspect, float near, float far)
{
	// scale controls zoom level

	aspect = (float)width / (float)height;

	this->projection = glm::ortho(
		-scale * aspect, scale * aspect,  // left, right
		-scale, scale,                    // bottom, top
		near, far						  // near, far			pogledaj ove parametre pazljivo
	); // top-left origin
	this->update();
}


Ray Camera::createRay(GLFWwindow* glfwWindow)
{
	double mouseX, mouseY, mouseZ;

	glfwGetCursorPos(glfwWindow, &mouseX, &mouseY); // cursor coordinates in regards to the window
	//std::cout << "\n\n\tMouseCOORDINATES   x = " << mouseX << " y = " << mouseY;
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

	Viewport* viewport = window->getViewportAtCursor(mouseX, mouseY);

	glm::ivec4 corners = viewport->getCorners(window);

	//std::cout << "\n corners.x = " << corners.x << " corners.y = " << corners.y << " corners.z = " << corners.z << " corners.w = " << corners.w;

	mouseX -= corners.x;
	mouseY = window->getHeight() - mouseY - corners.y; // coordinates in regards to the viewport


	mouseX = 2.0 * mouseX / width - 1.0;
	mouseY = 2.0 * mouseY / height - 1.0/* - viewport->getBottom()*/;  // NDC [-1.0, 1.0]
	//std::cout << "\n\tVIEWPORT COORDINATES   x = " << mouseX << " y = " << mouseY << "\tcorner.y "<< viewport->getBottom();

	glm::vec4 temp(mouseX, mouseY, -1.0, 1.0); // Homogeneous Clip Coordinates

	temp = glm::inverse(projection) * temp;
	temp = glm::vec4(temp.x, temp.y, -1.0, 0.0);	// Camera coordinates

	glm::vec4 A = (glm::inverse(glm::inverse(projection) * cameraMatrix) * temp); // World coordinates

	return Ray(position, glm::normalize(glm::vec3(A.x, A.y, A.z)));
}

void Camera::movement3D(Window* window, Viewport* viewport)
{

	std::vector<int>& keys = viewport->getKeys();
	std::vector<int>& keysProcessed = viewport->getKeysProcessed();
	std::vector<int>& buttons = viewport->getMouseButtons();
	std::vector<int>& buttonsProcessed = viewport->getMouseButtonsProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	//MOUSE MIDDLE ROTATE
	//MOUSE RIGHT PAN
	if (buttons[GLFW_MOUSE_BUTTON_MIDDLE] && keys[GLFW_KEY_LEFT_SHIFT])
	{

		glfwGetCursorPos(window->getGLFWwindow(), &posX, &posY);

		// Prevents camera from jumping on the first click
		if (firstClick || viewport->cursorWrapAround(window, posX, posY))
		{
			//glfwSetCursorPos(window, (width / 2), (height / 2));
			previousX = posX;
			previousY = posY;
			firstClick = false;
			//std::cout << "HAHAHAH";
			return;
		}
		float deltaX = (posX - previousX) / 150;
		float deltaY = (posY - previousY) / 150;

		//std::cout << deltaX << " " << posX << " " << previousX << "\n";
		position += -deltaX * glm::normalize(glm::cross(orientation, up));
		position += deltaY * up;

		previousX = posX;
		previousY = posY;
	}
	else if (buttons[GLFW_MOUSE_BUTTON_MIDDLE]) // ROTATE
	{
		//std::cout << "ASDASD";

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			//std::cout << "HAHAHAH";
			glfwSetCursorPos(window->getGLFWwindow(),
				viewport->getLeft() * window->getWidth() + (width / 2),
				viewport->getBottom() * window->getHeight() + (height / 2));
			firstClick = false;
		}


		double mouseX;
		double mouseY;

		glfwGetCursorPos(window->getGLFWwindow(), &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the orientation
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Decides whether or not the next vertical orientation is legal or not
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		// Rotates the orientation left and right
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		//std::cout << "\n orientation:" << orientation.x<<" "<<orientation.y << " " << orientation.z;

		glfwSetCursorPos(window->getGLFWwindow(),
			viewport->getLeft() * window->getWidth() + (width / 2),
			viewport->getBottom() * window->getHeight() + (height / 2)); //NEKADA IZBACITI OVO I STAVITI ROTXPREVIOUS I ROTYPREVIOUS
	}



	//RESET
	if (buttonsProcessed[GLFW_MOUSE_BUTTON_MIDDLE])
	{
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;

		if (glfwGetKey(window->getGLFWwindow(), GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
		{
			//std::cout << "asdasdasda";
			keys[GLFW_KEY_LEFT_SHIFT] = 0;
			buttonsProcessed[GLFW_MOUSE_BUTTON_MIDDLE] = 0; // makes it so that this if only goes through 1 iteration
		}
	}
}

void Camera::movement2D(Window* window, Viewport* viewport)
{

	std::vector<int>& keys = viewport->getKeys();
	std::vector<int>& keysProcessed = viewport->getKeysProcessed();
	std::vector<int>& buttons = viewport->getMouseButtons();
	std::vector<int>& buttonsProcessed = viewport->getMouseButtonsProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	if (keys[GLFW_KEY_LEFT_ALT])
		if (buttons[GLFW_MOUSE_BUTTON_RIGHT]) // PAN
		{
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window->getGLFWwindow(), &posX, &posY);

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				//glfwSetCursorPos(window, (width / 2), (height / 2));
				previousX = posX;
				previousY = posY;
				firstClick = false;
				//std::cout << "HAHAHAH";
				return;
			}
			float deltaX = (posX - previousX) / 150;
			float deltaY = (posY - previousY) / 150;

			//std::cout << deltaX << " " << posX << " " << previousX << "\n";
			position.x += -deltaX;
			position.y += deltaY;

			previousX = posX;
			previousY = posY;
		}
	//RESET
	if (!buttons[GLFW_MOUSE_BUTTON_RIGHT] && !keys[GLFW_KEY_G])
	{
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;

		if (glfwGetKey(window->getGLFWwindow(), GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
		{
			//std::cout << "asdasdasda";
			keys[GLFW_KEY_LEFT_ALT] = 0;
			buttonsProcessed[GLFW_MOUSE_BUTTON_LEFT] = 0; // makes it so that this if only goes through 1 iteration
			buttonsProcessed[GLFW_MOUSE_BUTTON_RIGHT] = 0; // makes it so that this if only goes through 1 iteration
		}
	}
}

void Camera::setCamera2D()
{
	std::cout << "SetCamera2D";

	// View from above, looking down -Z
	this->position = glm::vec3(0.0f, 0.0f, +3.0f); // Z = +1
	orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, -1.0f, 0.0f);

	this->update();


}
