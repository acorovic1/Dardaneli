#include "Camera.h"
#include "MyGUI.h"
#include"CameraManager.h"

Camera::Camera(int width, int height, glm::vec3 Position) 
{
	this->width = width;
	this->height = height;
	this->Position = Position;

	cameraSingleton->addCamera(this);
}

void Camera::setProjectionMatrix(glm::mat4& projection)
{
	this->Projection = projection;
}
void Camera::setProjectionMatrix(glm::mat4 projection)
{
	this->Projection = projection;
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



void Camera::Inputs(GLFWwindow* window, MyGUI& gui) {

	if(gui.getMode()==Mode::EDIT)
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetTime()>1) {
			Mesh *mesh = static_cast<Mesh*>(objectSingleton->getObject(gui.getObjectIndex()));
			gui.getVertexIndex()=mesh->extrudeVertex(gui.getVertexIndex());
			glfwSetTime(0);
			std::cout << "a";
		}



	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { // Go forward
		Position += speed * Orientation;
		//std::cout << "W";
		//Position += speed * glm::vec3(0.0f, 0.0f, -1.0f); //FPS camera
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));// Go left				//LAG
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {	// Go Backward
		Position += speed * -Orientation;
		//Position += speed * -glm::vec3(0.0f, 0.0f, -1.0f); //FPS camera
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {	// Go right
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Go up 
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // Go down
	{
		Position += speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Gotta go fast!
	{
		speed = 0.005f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) // Go normal
	{
		speed = 0.001f;
	}
	

	if(glfwGetKey(window,GLFW_KEY_LEFT_ALT)==GLFW_PRESS)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{


			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				glfwSetCursorPos(window, (width / 2), (height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &mouseX, &mouseY);

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

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetCursorPos(window, (width / 2), (height / 2));
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			{

				// Makes sure the next time the camera looks around it doesn't jump
				firstClick = true;
			}

	}

	
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		
		if (gui.getMode() == Mode::OBJECT)
		{

			objectBVHSingleton->getRoot()->Hit(CreateRay(window),gui.getObjectIndex());
			gui.SelectObject();

			if (gui.getObjectIndex() != -1 ) // -1 is the miss constant
			{
			std::cout << "\nSELECTED ---> " << objectSingleton->getObject(gui.getObjectIndex())->getName();
			}
		}
		else if (gui.getMode() == Mode::EDIT)
		{
			editModeBVHSingleton->getRoot()->Hit(CreateRay(window), gui.getVertexIndex());
		}
	}
	if (  glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		gui.AddMenu();		
	}


	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		gui.setGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		gui.setGizmoOperation(ImGuizmo::OPERATION::ROTATE);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		gui.setGizmoOperation(ImGuizmo::OPERATION::SCALE);
	}


}
