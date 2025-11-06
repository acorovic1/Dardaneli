#include "Camera.h"
#include "MyGUI.h"
#include "CameraManager.h"
#include "Window.h"

Camera::Camera(int width, int height, glm::vec3 position, std::string name)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
	Camera::name = name;

	fov = 45.0, near = 0.01f, far = 100.0f;

	cameraSingleton->addCamera(this);
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

void Camera::setOrthographicProjection()
{
	float orthoScale = 5.0f; // controls zoom level
	float aspect = (float)width / (float)height;

	this->projection = glm::ortho(
		-orthoScale * aspect, orthoScale * aspect,  // left, right
		-orthoScale, orthoScale,                    // bottom, top
		-100.0f, 100.0f                             // near, far
	); // top-left origin
	this->update();
}


Ray Camera::createRay(GLFWwindow* window)
{
	double mouseX, mouseY, mouseZ;

	glfwGetCursorPos(window, &mouseX, &mouseY); // Viewport Coordinates
	//std::cout << "VIEWPORT COORDINATES   x = " << mouseX << " y = " << mouseY << "\n";

	mouseX = 2.0 * mouseX / width - 1.0;
	mouseY = 1.0 - 2.0 * mouseY / height;  // NDC

	glm::vec4 temp(mouseX, mouseY, -1.0, 1.0); // Homogeneous Clip Coordinates

	temp = glm::inverse(projection) * temp;
	temp = glm::vec4(temp.x, temp.y, -1.0, 0.0);	// Camera coordinates

	glm::vec4 A = (glm::inverse(glm::inverse(projection) * cameraMatrix) * temp); // World coordinates

	return Ray(position, glm::normalize(glm::vec3(A.x, A.y, A.z)));
}

void Camera::movement3D(GLFWwindow* glfwWindow)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	std::vector<int>& buttons = window->getMouseButtons();
	std::vector<int>& buttonsProcessed = window->getMouseButtonsProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	//MOUSE LEFT ROTATE
	//MOUSE RIGHT PAN
	if (keys[GLFW_KEY_LEFT_ALT]) {
		if (buttons[GLFW_MOUSE_BUTTON_LEFT]) // ROTATE
		{
			//std::cout << "ASDASD";

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				//std::cout << "HAHAHAH";
				glfwSetCursorPos(window->getGLFWwindow(), (width / 2), (height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
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

			glfwSetCursorPos(window->getGLFWwindow(), (width / 2), (height / 2)); //NEKADA IZBACITI OVO I STAVITI ROTXPREVIOUS I ROTYPREVIOUS
		}
		else if (buttons[GLFW_MOUSE_BUTTON_RIGHT])
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
			position += -deltaX * glm::normalize(glm::cross(orientation, up));
			position += deltaY * up;

			previousX = posX;
			previousY = posY;
		}
	}

	//RESET
	if ((buttonsProcessed[GLFW_MOUSE_BUTTON_LEFT] && !buttons[GLFW_MOUSE_BUTTON_RIGHT]) ||
		(buttonsProcessed[GLFW_MOUSE_BUTTON_RIGHT] && !buttons[GLFW_MOUSE_BUTTON_LEFT]))
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

void Camera::movement2D(GLFWwindow* glfwWindow)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	std::vector<int>& buttons = window->getMouseButtons();
	std::vector<int>& buttonsProcessed = window->getMouseButtonsProcessed();
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
	if (!buttons[GLFW_MOUSE_BUTTON_RIGHT])
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
