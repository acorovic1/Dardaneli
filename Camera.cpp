#include "Camera.h"
#include "MyGUI.h"
#include"CameraManager.h"

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
float Camera::getFOV()const{ return fov; }
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// Retrieve the Camera instance stored in GLFW user pointer
	float fov= cameraSingleton->getCamera(0)->getFOV();

	
	fov -= static_cast<float>(yoffset);  // Modify private fov
	if (fov < 1.0f) fov = 1.0f;   // Clamp minimum zoom
	if (fov > 90.0f) fov = 90.0f; // Clamp maximum zoom

	cameraSingleton->getCamera(0)->setFOV(fov);
	std::cout << "Updated FOV: " << fov << std::endl;
}

void Camera::setScrollCallback(GLFWwindow*window){ glfwSetScrollCallback(window, scroll_callback); }


void Camera::Movement(GLFWwindow* window, MyGUI& gui)
{

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
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
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) 
		{
			

			

			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &posX, &posY);

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				//glfwSetCursorPos(window, (width / 2), (height / 2));
				previousX = posX;
				previousY = posY;
				firstClick = false;
				return;
			}
			float deltaX = (posX - previousX)/150;
			float deltaY = (posY - previousY)/150;
			
			std::cout << deltaX << " " << posX <<" "<< previousX <<"\n";
			Position += -deltaX * glm::normalize(glm::cross(Orientation, Up));
			Position += deltaY * Up;

			previousX = posX;
			previousY = posY;
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{

			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}

	}

}
void Camera::ObjectMode(GLFWwindow* window, MyGUI& gui)
{
	// SELECT
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (glfwGetTime() < 0.5)return;
		glfwSetTime(0);

		std::vector<int>& objectIndices = gui.getObjectIndex();
		
		int index = -1;
		objectBVHSingleton->getRoot()->Hit(CreateRay(window), index);
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			if (index == -1)return;
			objectIndices.erase(std::remove_if(objectIndices.begin(), objectIndices.end(), [index](int a) {return a == index; }), objectIndices.end());
			objectIndices.push_back(index);
			gui.SelectObject();
			std::cout << "\nMULTI SELECT ---> " << objectSingleton->getObject(index)->getName();
		}
		else if (index == -1)// -1 is the miss constant
		{
			//gui.getObjectIndex().assign({-1});
			objectIndices.clear();

		}
		else	
		{
			
			objectIndices.clear();
			objectIndices.push_back(index);
			std::cout << "\nSELECTED ---> " << objectSingleton->getObject(index)->getName();
			gui.SelectObject();
		}


		std::cout << "\n indices ";
		for (auto x : objectIndices)
			std::cout << x << " ";
	}

	// ADD MENU
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		gui.AddMenu();
	}

	// GIZMO OPERATION
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


	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &posX, &posY);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			//glfwSetCursorPos(window, (width / 2), (height / 2));
			previousX = posX;
			previousY = posY;
			firstClick = false;
			return;
		}
		float deltaX = (posX - previousX) / 150;
		float deltaY = (posY - previousY) / 150;


		/// FALI Y KRETNJA
		std::cout << deltaX << " " << posX << " " << previousX << "\n";
		auto offset = deltaX * glm::normalize(glm::cross(Orientation, Up));
		if(glfwGetKey(window,GLFW_KEY_X)==GLFW_PRESS)
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate(offset.x,0.0f,0.0f);
		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate(0.0f,offset.y, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate( 0.0f, 0.0f, offset.z);
		for (auto x : gui.getObjectIndex())
			objectSingleton->getObject(x)->Translate(offset);

		previousX = posX;
		previousY = posY;
	}
}
void Camera::EditMode(GLFWwindow* window, MyGUI& gui)
{
	// SELECT
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (glfwGetTime() < 0.5)return;
		glfwSetTime(0);

		std::vector<int>& vertexIndices = static_cast<Mesh*>(objectSingleton->getObject(gui.getObjectIndex()[gui.getObjectIndex().size()-1]))->getSelectedVertices();
		int index = -1;
			
		editModeBVHSingleton->getRoot()->Hit(CreateRay(window),index);

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			if (index == -1)return;
			vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index; }), vertexIndices.end());
			vertexIndices.push_back(index);
			std::cout << "\nMULTI SELECT ---> " << index;
		}else if (index == -1)// -1 is the miss constant
		{
			vertexIndices.clear();
		}
		else
		{
			vertexIndices.clear();
			vertexIndices.push_back(index);
			std::cout << "\nSELECTED ---> " << index;
		}
		std::cout << "\nSelected vertices ";
		for (auto x : vertexIndices)
			std::cout << x << " ";
		
	}

	// EXTRUDE
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetTime() > 1) {

		if (glfwGetTime() < 0.5)return;
		glfwSetTime(0);

		Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(gui.getObjectIndex()[gui.getObjectIndex().size()-1]));
		std::vector<int>& vertexIndices = mesh->getSelectedVertices();

		std::vector<int> temp(0);
		for (int i = 0; i < vertexIndices.size(); i++)
		{
			temp.push_back(mesh->extrudeVertex(vertexIndices[i]));
		}
		vertexIndices = temp;
		glfwSetTime(0);
		std::cout << "extrude vertex";
	}

}


void Camera::Inputs(GLFWwindow* window, MyGUI& gui) {

	Camera::Movement(window,gui);

	if (gui.getMode() == Mode::OBJECT)
		Camera::ObjectMode(window, gui);
	else if (gui.getMode() == Mode::EDIT)
		Camera::EditMode(window, gui);

}
