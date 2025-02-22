#include "Application.h"
#include "CameraManager.h"
#include "MyGUI.h"

Application* Application::instance = nullptr;

Application* app = Application::getInstance();

Application* Application::getInstance() {
	if (!instance)
		instance = new Application();
	return instance;
}

void Application::updateTranslate(glm::vec3 offset)
{
	translate[0] += offset.x;
	translate[1] += offset.y;
	translate[2] += offset.z;

}

void Application::updateVertexPosition(glm::vec3 offset)
{
	vertexPosition[0] += offset.x;
	vertexPosition[1] += offset.y;
	vertexPosition[2] += offset.z;
}



















void Application::ObjectMode(GLFWwindow* window, MyGUI& gui)
{
	static Camera* camera = cameraSingleton->getCamera(0);

	Window* classWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	std::vector<int>& mouseButtons = classWindow->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = classWindow->getMouseButtonsProcessed();
	std::vector<int>& keys = classWindow->getKeys();
	std::vector<int>& keysProcessed = classWindow->getKeysProcessed();
	bool& firstClick = classWindow->getFirstClick();
	double& posX = classWindow->getPosX();
	double& posY = classWindow->getPosY();
	double& previousX = classWindow->getPreviousX();
	double& previousY = classWindow->getPreviousY();
	
	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		if (keys[GLFW_KEY_LEFT_ALT])return;

		std::vector<int>& objectIndices = gui.getObjectIndex();

		int index = -1;
		objectBVHSingleton->getRoot()->Hit(camera->CreateRay(window), index);

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


		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;

		keys[GLFW_KEY_G] = 0;
		firstClick = true;
		keys[GLFW_KEY_X] = 0;
		keys[GLFW_KEY_Y] = 0;
		keys[GLFW_KEY_Z] = 0;
	}

	// ADD MENU
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		gui.AddMenu();

	}

	// GIZMO OPERATION
	if (keys[GLFW_KEY_1])
	{
		gui.setGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
		keys[GLFW_KEY_1] = 0;
	}
	if (keys[GLFW_KEY_2])
	{
		gui.setGizmoOperation(ImGuizmo::OPERATION::ROTATE);
		keys[GLFW_KEY_2] = 0;
	}
	if (keys[GLFW_KEY_3])
	{
		gui.setGizmoOperation(ImGuizmo::OPERATION::SCALE);
		keys[GLFW_KEY_3] = 0;
	}

	// OBJECT TRANSLATE
	if (keys[GLFW_KEY_G] == 1)
	{
		if (!gui.getObjectIndex().size())
		{
			keys[GLFW_KEY_G] = 0;
			return;
		}
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &posX, &posY);

		// Prevents  jumping on the first click
		if (firstClick)
		{
			//glfwSetCursorPos(window, (width / 2), (height / 2));
			previousX = posX;
			previousY = posY;
			firstClick = false;
			return;
		}
		float deltaX = (posX - previousX) / 150;
		float deltaY = (previousY - posY) / 150;


		/// FALI Y KRETNJA
		auto offset = deltaX * glm::normalize(glm::cross(camera->getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f))) + deltaY * glm::vec3(0.0f, 1.0f, 0.0f);


		std::cout << offset.x << " " << offset.y << " " << offset.z << "\n";

		if (keys[GLFW_KEY_X] == 1)
		{
			std::cout << "X";
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate(offset.x, 0.0f, 0.0f);
			app->updateTranslate(glm::vec3(offset.x, 0.0f, 0.0f));
		}
		else if (keys[GLFW_KEY_Y] == 1)
		{
			std::cout << "Y";

			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate(0.0f, offset.y, 0.0f);
			app->updateTranslate(glm::vec3(0.0f, offset.y, 0.0f));
		}
		else if (keys[GLFW_KEY_Z] == 1)
		{
			std::cout << "Z";
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate(0.0f, 0.0f, offset.z);
			app->updateTranslate(glm::vec3(0.0f, 0.0f, offset.z));
		}
		else
		{
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->Translate(offset);
			app->updateTranslate(offset);
		}

		objectBVHSingleton->Refit();
		previousX = posX;
		previousY = posY;
	}
}
void Application::EditMode(GLFWwindow* window, MyGUI& gui)
{

	Window* classWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	std::vector<int>& mouseButtons = classWindow->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = classWindow->getMouseButtonsProcessed();
	std::vector<int>& keys = classWindow->getKeys();
	std::vector<int>& keysProcessed = classWindow->getKeysProcessed();
	bool& firstClick = classWindow->getFirstClick();
	double& posX = classWindow->getPosX();
	double& posY = classWindow->getPosY();
	double& previousX = classWindow->getPreviousX();
	double& previousY = classWindow->getPreviousY();

	static Camera* camera = cameraSingleton->getCamera(0);
	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(gui.getObjectIndex()[gui.getObjectIndex().size() - 1]));
	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		if (keys[GLFW_KEY_LEFT_ALT])return;


		std::vector<int>& vertexIndices = mesh->getSelectedVertices();
		int index = -1;

		editModeBVHSingleton->getRoot()->Hit(camera->CreateRay(window), index);

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			if (index == -1)return;
			vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index; }), vertexIndices.end());
			vertexIndices.push_back(index);
			std::cout << "\nMULTI SELECT ---> " << index;
		}
		else if (index == -1)// -1 is the miss constant
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


		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;
		keys[GLFW_KEY_E] = 0;

	}

	// TRANSLATE
	if (keys[GLFW_KEY_G] == 1)
	{

		if (!mesh->getSelectedVertices().size())
		{
			keys[GLFW_KEY_G] = 0;
			return;
		}
		// Fetches the coordinates of the cursor
		std::cout << "Hello\n";
		glfwGetCursorPos(window, &posX, &posY);

		// Prevents  jumping on the first click
		if (firstClick)
		{

			previousX = posX;
			previousY = posY;
			firstClick = false;
			return;
		}
		float deltaX = (posX - previousX) / 150;
		float deltaY = (previousY - posY) / 150;



		auto offset = deltaX * glm::normalize(glm::cross(camera->getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f))) + deltaY * glm::vec3(0.0f, 1.0f, 0.0f);


		std::cout << offset.x << " " << offset.y << " " << offset.z << "\n";

		std::vector<Vertex>& vertices = mesh->getVerticesReference();
		if (keys[GLFW_KEY_X] == 1)
		{
			std::cout << "X";
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x].Translate(offset.x, 0.0f, 0.0f);
				mesh->UpdateData(x);
			}

			app->updateVertexPosition(glm::vec3(offset.x, 0.0f, 0.0f));
		}
		else if (keys[GLFW_KEY_Y] == 1)
		{
			std::cout << "Y";

			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x].Translate(0.0f, offset.y, 0.0f);
				mesh->UpdateData(x);
			}
			app->updateVertexPosition(glm::vec3(0.0f, offset.y, 0.0f));
		}
		else if (keys[GLFW_KEY_Z] == 1)
		{
			std::cout << "Z";
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x].Translate(0.0f, 0.0f, offset.z);
				mesh->UpdateData(x);
			}
			app->updateVertexPosition(glm::vec3(0.0f, 0.0f, offset.z));
		}
		else
		{
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x].Translate(offset);
				mesh->UpdateData(x);
			}
			app->updateVertexPosition(offset);
		}

		editModeBVHSingleton->Refit(*mesh);

		previousX = posX;
		previousY = posY;
	}

	// EXTRUDE
	if (keys[GLFW_KEY_E]) {

		std::vector<int>& vertexIndices = mesh->getSelectedVertices();

		std::vector<int> temp(0);
		for (int i = 0; i < vertexIndices.size(); i++)
		{
			temp.push_back(mesh->extrudeVertex(vertexIndices[i]));
		}
		vertexIndices = temp;

		editModeBVHSingleton->BuildBottomUp(*mesh);

		std::cout << "extrude vertex";

		keys[GLFW_KEY_E] = 0;
		keys[GLFW_KEY_G] = 1;
	}

}


void Application::Inputs(GLFWwindow* window, MyGUI& gui) {

	static Camera* camera = cameraSingleton->getCamera(0);
	camera->Movement(window, gui);

	if (gui.getMode() == Mode::OBJECT)
		Application::ObjectMode(window, gui);
	else if (gui.getMode() == Mode::EDIT)
		Application::EditMode(window, gui);

}



