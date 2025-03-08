#include "Camera.h"
#include "MyGUI.h"
#include "CameraManager.h"
#include "Window.h"

Camera::Camera(int width, int height, glm::vec3 Position) 
{
	Camera::width = width;
	Camera::height = height;
	Camera::Position = Position;
	
	fov = 45.0,near=0.01f,far=100.0f;
	posX=0, posY=0, previousX=0, previousY = 0;

	cameraSingleton->addCamera(this);
}

void Camera::setProjectionMatrix(float fov, float aspect, float near, float far)
{
	Camera::fov = fov;
	Camera::near = near;
	Camera::far = far;
	this->Projection = glm::perspective(glm::radians(fov),aspect,near,far);
}

glm::mat4 Camera::getViewMatrix()const {
	return glm::lookAt(Position, Position + Orientation, Up);
}
glm::mat4 Camera::getProjectionMatrix()const {
	return Projection;
}
void Camera::Update()
{
	cameraMatrix = Projection * getViewMatrix(); // projection * view
}
void Camera::CameraUniform(Shader& shader, const char* uniform) {
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}
int Camera::getWidth()const { return width; }
int Camera::getHeight()const { return height; }
glm::vec3 Camera::getPosition()const { return Position; }

glm::vec3 Camera::getOrientation() const
{
	return Orientation;
}


void Camera::setWidth(int width) { Camera::width = width; }
void Camera::setHeight(int height) { Camera::height = height; }



Ray Camera::CreateRay(GLFWwindow* window)
{
	double mouseX, mouseY,mouseZ;

	glfwGetCursorPos(window, &mouseX, &mouseY); // Viewport Coordinates
	//std::cout << "VIEWPORT COORDINATES   x = " << mouseX << " y = " << mouseY << "\n";

	mouseX = 2.0 * mouseX / width - 1.0;
	mouseY = 1.0 - 2.0 * mouseY / height;  // NDC

	glm::vec4 temp(mouseX, mouseY, -1.0,1.0); // Homogeneous Clip Coordinates

	temp = glm::inverse(Projection) * temp;
	temp = glm::vec4(temp.x, temp.y, -1.0, 0.0);	// Camera coordinates

	glm::vec4 A = (glm::inverse(glm::inverse(Projection) * cameraMatrix) * temp); // World coordinates

	return Ray(Position,glm::normalize(glm::vec3(A.x, A.y, A.z)));
}

void Camera::setFOV(float fov) 
{
	setProjectionMatrix(fov, float(getWidth()) / float(getHeight()), near, far);
}
void Camera::setPosition(glm::vec3 position)
{
	Position = position;
}
float Camera::getFOV()const{ return fov; }





void Camera::Movement(GLFWwindow* glfwWindow, MyGUI& gui)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	std::vector<int>& buttons = window->getMouseButtons();
	std::vector<int>& buttonsProcessed = window->getMouseButtonsProcessed();

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
				glfwSetCursorPos(window->GetWindow(), (width / 2), (height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window->GetWindow(), &mouseX, &mouseY);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
			float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

			// Calculates upcoming vertical change in the Orientation
			glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

			// Decides whether or not the next vertical Orientation is legal or not
			if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				Orientation = newOrientation;
			}

			// Rotates the Orientation left and right
			Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

			glfwSetCursorPos(window->GetWindow(), (width / 2), (height / 2)); //NEKADA IZBACITI OVO I STAVITI ROTXPREVIOUS I ROTYPREVIOUS

		}
		else if (buttons[GLFW_MOUSE_BUTTON_RIGHT])
		{

			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window->GetWindow(), &posX, &posY);

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
			Position += -deltaX * glm::normalize(glm::cross(Orientation, Up));
			Position += deltaY * Up;

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

		if (glfwGetKey(window->GetWindow(), GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
		{
			//std::cout << "asdasdasda";
			keys[GLFW_KEY_LEFT_ALT] = 0;
			buttonsProcessed[GLFW_MOUSE_BUTTON_LEFT] = 0; // makes it so that this if only goes through 1 iteration
			buttonsProcessed[GLFW_MOUSE_BUTTON_RIGHT] = 0; // makes it so that this if only goes through 1 iteration

		}

	}

}