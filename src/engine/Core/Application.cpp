#include "Application.h"
#include "CameraManager.h"
#include "MyGUI.h"
#include "Mesh/DFace.h"
#include "Mesh/DLoop.h"
#include "GeometryUtils.h"
#include "Window.h"
#include <Improved/VertexBVHImproved.h>
#include <Improved/EdgeBVHImproved.h>
#include <Improved/FaceBVHImproved.h>
#include <Improved/ObjectModeBVHImproved.h>



Application* Application::instance = nullptr;

Application* app = Application::getInstance();

Application* Application::getInstance() {
	if (!instance)
		instance = new Application();
	return instance;
}

void Application::setSelectMode(SelectMode mode)
{
	selectMode = mode;
}

void Application::updateGUI(glm::vec3 offset, MyGUI& gui, Operation op)
{
	if (op == Operation::TRANSLATE)
	{

		gui.position[0] += offset.x;
		gui.position[1] += offset.y;
		gui.position[2] += offset.z;
	}
	else if (op == Operation::ROTATE)
	{
		
	}
	else if (op == Operation::SCALE)
	{
		
	}

}




void Application::objectMode(Window* window)
{
	static Camera* camera = cameraSingleton->getCamera(0);

	std::vector<int>& mouseButtons = window->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = window->getMouseButtonsProcessed();
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	GLFWwindow* glfwWindow = window->getGLFWwindow();
	MyGUI& gui = window->getGui();

	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		if (keys[GLFW_KEY_LEFT_ALT])return;
		if (keys[GLFW_KEY_G])
		{
			objectBVHImprovedSingleton->Refit();
			keys[GLFW_KEY_G] = 0;

			return;
		}

		std::vector<int> indexVec;
		objectBVHImprovedSingleton->getRoot()->Hit(camera->createRay(glfwWindow), indexVec);

		int index = indexVec[0];
		if (indexVec.size() > 1) // if size == 1 it is a miss
		{
			indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());

			if (indexVec.size())
			{

				index = indexVec[0];
				if (indexVec.size() > 1)
				{
					auto cameraPosition = cameraSingleton->getCamera(0)->getPosition();
					auto closestPosition = glm::distance(cameraPosition, objectSingleton->getObject(indexVec[0])->getPosition());

					for (int i = 1; i < indexVec.size(); i++)
					{
						auto position = glm::distance(cameraPosition, objectSingleton->getObject(indexVec[i])->getPosition());
						if (position < closestPosition)
						{
							closestPosition = position;
							index = indexVec[i];
						}
					}
				}
			}
		}

		if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			if (index == -1)return;
			selectedObjects.erase(std::remove_if(selectedObjects.begin(), selectedObjects.end(), [index](int a) {return a == index; }), selectedObjects.end());
			selectedObjects.push_back(index);
			gui.selectObject();
			std::cout << "\nMULTI SELECT ---> " << objectSingleton->getObject(index)->getName();
		}
		else if (index == -1)// -1 is the miss constant
		{
			selectedObjects.clear();
		}
		else
		{
			selectedObjects.clear();
			selectedObjects.push_back(index);
			std::cout << "\nSELECTED ---> " << objectSingleton->getObject(index)->getName();
			gui.selectObject();
		}

		//std::cout << "\n indices ";
		//for (auto x : selectedObjects)
		//	std::cout << x << " ";

		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;

		firstClick = true;
		keys[GLFW_KEY_X] = 0;
		keys[GLFW_KEY_Y] = 0;
		keys[GLFW_KEY_Z] = 0;
	}

	// ADD MENU
	if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		gui.showAddMenu();
	}

	if (keys[GLFW_KEY_TAB])
	{
		keys[GLFW_KEY_TAB] = 0;

		Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getActiveObject());
		if (mesh == nullptr) return;
		mode = Mode::EDIT;

		VertexBVHImprovedSingleton->BuildBottomUp(mesh);
		EdgeBVHImprovedSingleton->BuildBottomUp(*mesh);
		FaceBVHImprovedSingleton->BuildBottomUp(*mesh);
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
		glfwGetCursorPos(glfwWindow, &posX, &posY);

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

		if (keys[GLFW_KEY_X] == 1)
		{
			std::cout << "X";
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->translate(offset.x, 0.0f, 0.0f);
			app->updateGUI(glm::vec3(offset.x, 0.0f, 0.0f), gui,Operation::TRANSLATE);
		}
		else if (keys[GLFW_KEY_Y] == 1)
		{
			std::cout << "Y";

			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->translate(0.0f, offset.y, 0.0f);
			app->updateGUI(glm::vec3(0.0f, offset.y, 0.0f), gui,Operation::TRANSLATE);
		}
		else if (keys[GLFW_KEY_Z] == 1)
		{
			std::cout << "Z";
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->translate(0.0f, 0.0f, offset.z);
			app->updateGUI(glm::vec3(0.0f, 0.0f, offset.z), gui,Operation::TRANSLATE);
		}
		else
		{
			for (auto x : gui.getObjectIndex())
				objectSingleton->getObject(x)->translate(offset);
			app->updateGUI(offset, gui,Operation::TRANSLATE);
		}

		previousX = posX;
		previousY = posY;
	}
}
void Application::editMode(Window* window)
{

	std::vector<int>& mouseButtons = window->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = window->getMouseButtonsProcessed();
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	static Camera* camera = cameraSingleton->getCamera(0);
	GLFWwindow* glfwWindow = window->getGLFWwindow();
	MyGUI& gui = window->getGui();

	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getActiveObject());

	if (!mesh)return;

	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		slide = false;
		firstClick = true;
		if (keys[GLFW_KEY_LEFT_ALT])return;
		keys[GLFW_KEY_S] = 0;
		keys[GLFW_KEY_X] = 0;
		keys[GLFW_KEY_Y] = 0;
		keys[GLFW_KEY_Z] = 0;
		if (keys[GLFW_KEY_G])
		{
			VertexBVHImprovedSingleton->Refit(mesh);
			EdgeBVHImprovedSingleton->Refit(*mesh);
			FaceBVHImprovedSingleton->Refit(*mesh);
			keys[GLFW_KEY_G] = 0;
			return;
		}

		std::vector<int>& vertexIndices = mesh->getSelectedVertices();
		std::vector<DVertex*>& verticesReference = mesh->getVertices();
		std::vector<int> indexVec;
		std::vector<int> index = { -1 };

		if (selectMode == SelectMode::VERTEX)
		{
			VertexBVHImprovedSingleton->getRoot()->Hit(camera->createRay(glfwWindow), indexVec);

			if (indexVec.size() > 1) // if size == 1 it is a miss
			{
				indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());
				indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), std::numeric_limits<unsigned int>::max()), indexVec.end());

				if (indexVec.size()) // discards all the "hits" beyond the first one.. makes it so the ray "stops after the first hit"
				{
					index = { indexVec[0] };

					auto cameraPosition = cameraSingleton->getCamera(0)->getPosition();
					const auto& v = mesh->getVerticesCopy();
					auto closestPosition = glm::distance(cameraPosition, v[index[0]].position);

					for (int i = 1; i < indexVec.size(); i++)
					{
						auto position = glm::distance(cameraPosition, v[indexVec[i]].position);
						if (position < closestPosition)
						{
							closestPosition = position;
							index = { indexVec[i] };
						}
					}
				}
			}


			if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
			{
				if (index[0] == -1)return;
				vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index[0]; }), vertexIndices.end());
				vertexIndices.push_back(index[0]);
				std::cout << "\nMULTI SELECT ---> " << index[0];
			}
			else if (index[0] == -1)// normal select .............		-1 is the miss constant
			{
				vertexIndices.clear();
			}
			else
			{
				vertexIndices.clear();
				vertexIndices.push_back(index[0]);
				std::cout << "\nSELECTED ---> " << index[0];
			}
		}

		if (selectMode == SelectMode::EDGE)
		{
			index.clear();

			std::vector<DEdge*> edgesHit;
			EdgeBVHImprovedSingleton->getRoot()->Hit(camera->createRay(glfwWindow), edgesHit);

			for (auto& edge : edgesHit)
			{
				if (!edge)continue;
				auto temp = mesh->getEdgeIndices(edge);
				indexVec.push_back(temp.first);
				indexVec.push_back(temp.second);

			}

			std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();
			DEdge* selectedEdge = nullptr;

			if (indexVec.size())
			{

				// discards all the "hits" beyond the first one.. makes it so the ray "stops after the first hit"

					//std::cout << "\n\n\nVEC SIZE " << indexVec.size();
				index = { indexVec[0],indexVec[1] };

				auto cameraPosition = cameraSingleton->getCamera(0)->getPosition();
				const auto& v = mesh->getVerticesCopy();
				auto v0 = v[indexVec[0]].position;
				auto v1 = v[indexVec[1]].position;
				auto midPoint = glm::vec3((v0.x + v1.x) / 2, (v0.y + v1.y) / 2, (v0.z - v1.z) / 2);
				auto closestPosition = glm::distance(cameraPosition, midPoint);

				for (int i = 2; i < indexVec.size(); i += 2)
				{
					v0 = v[indexVec[i]].position;
					v1 = v[indexVec[i + 1]].position;
					midPoint = glm::vec3(fabs(v0.x - v1.x), fabs(v0.y - v1.y), fabs(v0.z - v1.z));
					auto position = glm::distance(cameraPosition, midPoint);
					if (position < closestPosition)
					{
						closestPosition = position;
						index = { indexVec[i],indexVec[i + 1] };
					}
				}

				selectedEdge = mesh->getEdge(index[0], index[1]);
			}



			if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
			{
				if (!selectedEdge)return;

				auto it = std::find(selectedEdges.begin(), selectedEdges.end(), selectedEdge);
				if (it != selectedEdges.end())
					selectedEdges.erase(it);

				selectedEdges.push_back(selectedEdge);

				vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index[0]; }), vertexIndices.end());
				vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index[1]; }), vertexIndices.end());

				vertexIndices.push_back(index[0]);
				vertexIndices.push_back(index[1]);


				std::cout << "\nMULTI SELECT ---> " << index[0] << " " << index[1];


			}
			else if (!indexVec.size())// normal select ..... miss 
			{
				vertexIndices.clear();
				selectedEdges.clear();
			}
			else
			{
				if (!selectedEdge)return;

				vertexIndices.clear();
				selectedEdges.clear();

				vertexIndices.push_back(index[0]);
				vertexIndices.push_back(index[1]);

				selectedEdges.push_back(selectedEdge);

				std::cout << "\nSELECTED ---> " << index[0] << " " << index[1];
			}
		}

		if (selectMode == SelectMode::FACE)
		{
			index.clear();

			std::vector<DFace*>facesHit;
			FaceBVHImprovedSingleton->getRoot()->Hit(camera->createRay(glfwWindow), facesHit);
			std::vector<DFace*>& selectedFaces = mesh->getSelectedFaces();
			int numberOfVerticesInLastFace = 0;

			DFace* selectedFace = nullptr;

			for (auto& face : facesHit)
			{
				if (!face)continue;

				auto temp = mesh->getFaceIndices(face);

				indexVec.push_back(face->getVertices().size());
				indexVec.insert(indexVec.end(), temp.begin(), temp.end());

			}
			/*std::cout << "\n\n ";
			for (auto x : indexVec)
				std::cout << " " << x;
				std::cout << "\n\n ";*/

			if (indexVec.size()) // if size == 1 it is a miss
			{
				//indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());
				//indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), std::numeric_limits<unsigned int>::max()), indexVec.end());

				std::cout << "\n\n\t indexVec content:\t";
				for (auto x : indexVec)
					std::cout << " " << x;
				std::cout << "\n ";

				// discards all the "hits" beyond the first one.. makes it so the ray "stops after the first hit"

				for (int i = 1;i <= indexVec[0];i++)
					index.push_back(indexVec[i]);


				auto cameraPosition = cameraSingleton->getCamera(0)->getPosition();

				const auto& v = mesh->getVerticesCopy();

				auto midPoint = glm::vec3(0.0f, 0.0f, 0.0f);
				for (int i = 1;i < indexVec[0];i++)
				{
					midPoint = midPoint + v[indexVec[i]].position;
				}
				midPoint = midPoint / float(indexVec[0]);


				auto closestPosition = glm::distance(cameraPosition, midPoint);

				for (int i = 0; i < indexVec.size(); i += indexVec[i] + 1)
				{
					midPoint = glm::vec3(0.0f, 0.0f, 0.0f);
					for (int j = i + 1;j <= i + indexVec[i];j++)
					{
						midPoint = midPoint + v[indexVec[j]].position;

					}
					midPoint = midPoint / float(indexVec[i]);
					auto position = glm::distance(cameraPosition, midPoint);
					if (position < closestPosition)
					{
						closestPosition = position;
						index.clear();
						for (int j = i + 1;j <= i + indexVec[i];j++)
						{
							index.push_back(indexVec[j]);

						};
						numberOfVerticesInLastFace = indexVec[i];
					}
				}

				selectedFace = mesh->getFace(std::unordered_set<int>(index.begin(), index.end()));
			}

			std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();

			if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
			{
				if (!selectedFace)return;

				//add the face to selectedFaces atribute
				auto it = std::find(selectedFaces.begin(), selectedFaces.end(), selectedFace);
				if (it != selectedFaces.end())
					selectedFaces.erase(it);
				selectedFaces.push_back(selectedFace);

				for (auto& x : selectedFace->getEdges())
				{
					auto edgeIt = std::find(selectedEdges.begin(), selectedEdges.end(), x);
					if (edgeIt != selectedEdges.end())
						selectedEdges.erase(edgeIt);
					selectedEdges.push_back(x);
				}

				for (auto x : index)
					vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [x](int a) {return a == x; }), vertexIndices.end());

				vertexIndices.insert(vertexIndices.end(), index.begin(), index.end());
				std::cout << "\nMULTI SELECT ---> ";
				for (auto x : index)
					std::cout << x << " ";
			}
			else if (!selectedFace)// miss
			{
				vertexIndices.clear();
				selectedFaces.clear();
				selectedEdges.clear();
			}
			else
			{
				vertexIndices.clear();
				selectedFaces.clear();
				selectedEdges.clear();
				vertexIndices = index;

				selectedFaces.push_back(selectedFace);
				for (auto& x : selectedFace->getEdges())
					selectedEdges.push_back(x);


				std::cout << "\nSELECTED ---> ";
				for (auto x : index)
					std::cout << x << " ";
			}
		}


		std::cout << "\nSelected vertices ";
		for (auto x : vertexIndices)
			std::cout << x << " ";

		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;
		keys[GLFW_KEY_E] = 0;
	}

	// SELECT MODE
	if (keys[GLFW_KEY_1])
	{
		keys[GLFW_KEY_1] = 0;
		selectMode = SelectMode::VERTEX;
	}
	if (keys[GLFW_KEY_2])
	{
		keys[GLFW_KEY_2] = 0;
		selectMode = SelectMode::EDGE;
	}
	if (keys[GLFW_KEY_3])
	{
		keys[GLFW_KEY_3] = 0;
		selectMode = SelectMode::FACE;
	}

	if (keys[GLFW_KEY_TAB])
	{
		keys[GLFW_KEY_TAB] = 0;
		mode = Mode::OBJECT;

		objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
	}

	// SELECT ALL
	if (keys[GLFW_KEY_A])
	{
		auto& selectedVertices = mesh->getSelectedVertices();
		auto& selectedEdges = mesh->getSelectedEdges();
		auto& selectedFaces = mesh->getSelectedFaces();


		selectedVertices.clear();
		selectedVertices.resize(mesh->getVertices().size());
		std::iota(selectedVertices.begin(), selectedVertices.end(), 0);

		// vector assign replaces all the contents of the vector before inserting new values
		auto selectedEdgesSet = mesh->getAllEdges();
		selectedEdges.assign(selectedEdgesSet.begin(), selectedEdgesSet.end());

		auto selectedFacesSet = mesh->getAllFaces();
		selectedFaces.assign(selectedFacesSet.begin(), selectedFacesSet.end());

		keys[GLFW_KEY_A] = 0;
	}

	// SCALE
	if (keys[GLFW_KEY_S])
	{

		glfwGetCursorPos(glfwWindow, &posX, &posY);

		static std::vector<glm::vec3> directions;

		if (firstClick)
		{
			directions.clear();
			for (auto& edge : mesh->getSelectedEdges())
				directions.push_back(glm::normalize(edge->v1->position - edge->v2->position));

			previousX = posX;
			previousY = posY;
			firstClick = false;
			return;
		}
		float deltaX = (posX - previousX) / 150;
		float deltaY = (previousY - posY) / 150;

		auto edges = mesh->getSelectedEdges();
		glm::vec3 offset;
		for (int i = 0;i < edges.size();i++)
		{
			offset = deltaX * directions[i];
			edges[i]->v1->translate(offset);
			edges[i]->v2->translate(-offset);
			mesh->updateVertexBuffer(mesh->getVertexIndex(edges[i]->v1));
			mesh->updateVertexBuffer(mesh->getVertexIndex(edges[i]->v2));
		}
		std::cout << "\nSCALE";
		previousX = posX;
		previousY = posY;
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

		glfwGetCursorPos(glfwWindow, &posX, &posY);

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

		std::vector<DVertex*>& vertices = mesh->getVertices();
		if (keys[GLFW_KEY_X] == 1)
		{
			std::cout << "X";
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->translate(offset.x, 0.0f, 0.0f);
				mesh->updateVertexBuffer(x);
			}

			app->updateGUI(glm::vec3(offset.x, 0.0f, 0.0f), gui,Operation::TRANSLATE);
		}
		else if (keys[GLFW_KEY_Y] == 1)
		{
			std::cout << "Y";

			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->translate(0.0f, offset.y, 0.0f);
				mesh->updateVertexBuffer(x);
			}
			app->updateGUI(glm::vec3(0.0f, offset.y, 0.0f), gui,Operation::TRANSLATE);
		}
		else if (keys[GLFW_KEY_Z] == 1)
		{
			std::cout << "Z";
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->translate(0.0f, 0.0f, offset.z);
				mesh->updateVertexBuffer(x);
			}
			app->updateGUI(glm::vec3(0.0f, 0.0f, offset.z), gui,Operation::TRANSLATE);
		}
		else
		{
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->translate(offset);
				mesh->updateVertexBuffer(x);
			}
			app->updateGUI(offset, gui,Operation::TRANSLATE);
		}

		previousX = posX;
		previousY = posY;
	}

	// SLIDE
	if (keys[GLFW_KEY_G] == 2)
	{
		static glm::vec2 drag;
		static glm::vec2 dragPrev;
		if (!mesh->getSelectedVertices().size())
		{
			keys[GLFW_KEY_G] = 0;
			return;
		}
		// Fetches the coordinates of the cursor

		glfwGetCursorPos(glfwWindow, &posX, &posY);


		auto& tempVertices = mesh->getVertices();

		// Prevents  jumping on the first click
		if (!slide)
		{

			slide = true;

			slideDirections.clear();
			startPositions.clear();
			slideUnProjectedDirections.clear();
			bestDirection = std::vector<glm::vec3>(mesh->getSelectedVertices().size(), glm::vec3(0.0f));

			for (auto x : mesh->getSelectedVertices())
			{
				auto neighbours = tempVertices[x]->getAdjecentVertices();
				startPositions.push_back(tempVertices[x]->position);
				slideLengths.push_back(mesh->getSlideClampMax(neighbours));
				slideDirections.push_back(mesh->getSlideDirections(tempVertices[x], neighbours));
				slideUnProjectedDirections.push_back(mesh->getSlideUnprojectedDirections(tempVertices[x], neighbours));
			}

			previousX = posX;
			previousY = posY;

			slideStartX = posX;
			slideStartY = posY;

			std::cout << "\n\n\t slideStartX " << slideStartX << "\n\t slideStartY " << slideStartY;

			dragPrev = drag = glm::vec2(0.0f, 0.0f);

			return;
		}
		float deltaX = (posX - previousX) / 150;
		float deltaY = (previousY - posY) / 150;



		drag.x = (posX - slideStartX)/* * (drag.x - dragPrev.x >= 0) ? 1 : -1*/;
		drag.y = (slideStartY - posY)/* * (dragPrev.y - drag.y >= 0) ? 1 : -1*/;



		auto selectedVertices = mesh->getSelectedVertices();

		for (int i = 0;i < selectedVertices.size();i++)
		{
			auto bestDir = GeometryUtils::bestSlideDirection(slideDirections[i], slideUnProjectedDirections[i], drag);
			if (!glm::all(glm::epsilonEqual(bestDir, bestDirection[i], 0.001f)))
			{
				bestDirection[i] = bestDir;

				tempVertices[selectedVertices[i]]->position = startPositions[i];
				/*		if (i == selectedVertices.size() - 1)
							app->updateGUI(deltaX * bestDir);*/
			}
			int j = 0;
			for (j = 0;j < slideDirections[i].size();j++)
			{
				if (glm::all(glm::epsilonEqual(bestDir, slideDirections[i][j], 0.001f)) || glm::all(glm::epsilonEqual(bestDir, -slideDirections[i][j], 0.001f)))
					break;
			}

			tempVertices[selectedVertices[i]]->translate(fabs(deltaX) * bestDir);


			float projectedLength = glm::dot(tempVertices[selectedVertices[i]]->position - startPositions[i], bestDir); // scalar movement along direction
			float maxLength = glm::length(slideLengths[i][j] - startPositions[i]);
			projectedLength = glm::clamp(projectedLength, 0.0f, maxLength);

			tempVertices[selectedVertices[i]]->position = startPositions[i] + bestDir * projectedLength;

			mesh->updateVertexBuffer(selectedVertices[i]);

			if (i == selectedVertices.size() - 1)
				app->updateGUI(bestDir * projectedLength, gui,Operation::TRANSLATE);
		}


		previousX = posX;
		previousY = posY;
	}

	// INSET
	if (glfwGetKey(glfwWindow, GLFW_KEY_I) == GLFW_PRESS)
	{
		mesh->insetIndividual(mesh->getSelectedFaces());
		keys[GLFW_KEY_S] = 1;
	}

	// SEPARATE
	if (keys[GLFW_KEY_Y] == 1)
	{

		std::cout << "\n\n\t [Operation] Separate \n";
		mesh->separate(mesh->getSelectedFaces());

		keys[GLFW_KEY_Y] = 0;
		keys[GLFW_KEY_G] = 1;
	}

	// DUPLICATE
	if (keys[GLFW_KEY_D] && keys[GLFW_KEY_LEFT_SHIFT])
	{
		if (selectMode == SelectMode::VERTEX)
		{
			mesh->duplicateVertices(mesh->getSelectedVertices(), true);
		}
		else if (selectMode == SelectMode::EDGE)
		{
			mesh->duplicateEdges(mesh->getSelectedEdges(), true);
		}
		else if (selectMode == SelectMode::FACE)
		{

			mesh->duplicateFaces(mesh->getSelectedFaces(), true);

		}

		keys[GLFW_KEY_D] = 0;
		keys[GLFW_KEY_LEFT_SHIFT] = 0;
		keys[GLFW_KEY_G] = 1;
	}

	// DELETE
	if (glfwGetKey(glfwWindow, GLFW_KEY_X) == GLFW_PRESS)
	{

		gui.showDeleteMenu();

	}

	// EXTRUDE
	if (glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS) {

		gui.showExtrudeMenu();

		keys[GLFW_KEY_E] = 0;
		keys[GLFW_KEY_G] = 1;
	}

	// FILL
	if (keys[GLFW_KEY_F])
	{

		auto& temp = mesh->getSelectedVertices();
		if (temp.size() == 2)
			mesh->edgeFill(temp);
		else
			mesh->faceFill(temp, false, true);
		keys[GLFW_KEY_F] = 0;
	}

	// POKE
	if (keys[GLFW_KEY_P])
	{
		mesh->pokeFaces(mesh->getSelectedFaces(), true);
		keys[GLFW_KEY_P] = 0;
	}

	// TRIANGULATE
	if (keys[GLFW_KEY_T] && keys[GLFW_KEY_LEFT_CONTROL])
	{
		mesh->triangulateFaces(mesh->getSelectedFaces(), true);
		keys[GLFW_KEY_T] = 0;
		keys[GLFW_KEY_LEFT_CONTROL] = 0;
	}

	// TRIS TO QUADS
	if (keys[GLFW_KEY_T] && keys[GLFW_KEY_LEFT_ALT])

	{
		auto temp = mesh->getSelectedFaces();

		std::unordered_set < DFace*> faces{ temp.begin(),temp.end() };

		mesh->trisToQuads(faces, true);
		keys[GLFW_KEY_T] = 0;
		keys[GLFW_KEY_LEFT_ALT] = 0;
	}

	// BRIDGE FACES
	if (keys[GLFW_KEY_B])
	{
		auto& selectedFaces = mesh->getSelectedFaces();
		DFace* faceA = selectedFaces.back();
		selectedFaces.pop_back();
		DFace* faceB = selectedFaces.back();

		mesh->bridgeFaces(faceA, faceB, true);

		keys[GLFW_KEY_B] = 0;
	}

	// FLIP NORMALS
	if (keys[GLFW_KEY_N] && keys[GLFW_KEY_LEFT_ALT])
	{
		std::cout << "\n\n\t [Operation] Flip normals \n";
		mesh->flipFaceNormals(mesh->getSelectedFaces());

		keys[GLFW_KEY_N] = 0;
		keys[GLFW_KEY_LEFT_ALT] = 0;
	}

	// LINEAR SUBDIVISION
	if (keys[GLFW_KEY_S] && keys[GLFW_KEY_LEFT_CONTROL])
	{
		mesh->linearSubdivision();
		keys[GLFW_KEY_S] = 0;
		keys[GLFW_KEY_LEFT_CONTROL] = 0;
	}

	// LOOP CUT
	if (keys[GLFW_KEY_R] && keys[GLFW_KEY_LEFT_CONTROL])
	{
		mesh->loopCut(mesh->getSelectedEdges().back(), 1);
		keys[GLFW_KEY_R] = 0;
		keys[GLFW_KEY_LEFT_CONTROL] = 0;
	}

	// MERGE VERTICES
	if (keys[GLFW_KEY_M] && keys[GLFW_KEY_LEFT_CONTROL])
	{
		mesh->mergeVertices(mesh->getSelectedVertices());
		keys[GLFW_KEY_M] = 0;
		keys[GLFW_KEY_LEFT_CONTROL] = 0;
	}

}
void Application::uVMode(Window* window)
{


	std::vector<int>& mouseButtons = window->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = window->getMouseButtonsProcessed();
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	GLFWwindow* glfwWindow = window->getGLFWwindow();
	MyGUI& gui = window->getGui();
	static Camera* camera = cameraSingleton->getCamera("UV");
	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getActiveObject());
	if (!mesh)return;

	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		firstClick = true;

		if (keys[GLFW_KEY_G])
		{
			UVVertexBVHSingleton->Refit(*mesh);
			//EdgeBVHSingleton->Refit(*mesh);
			//FaceBVHSingleton->Refit(*mesh);

			mesh->buildGPUVertices();
			keys[GLFW_KEY_G] = 0;
			return;
		}

		auto& vertexIndicesss = mesh->getSelectedUVs();
		auto& selectedUVs = mesh->getSelectedUVs();
		auto& uvReference = mesh->getUVCoords();
		std::vector<int> indexVec;
		std::vector<int> index = { -1 };


		UVVertexBVHSingleton->getRoot()->Hit(camera->createRay(glfwWindow), indexVec);



		if (indexVec.size() > 1) // if size == 1 it is a miss
		{
			indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());
			indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), std::numeric_limits<unsigned int>::max()), indexVec.end());

			if (indexVec.size())
				index[0] = indexVec[0];
			else index[0] = -1;
		}


		if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
		{
			if (index[0] == -1)return;
			// probably does not need this remove_if, a simple find will suffice... check the logic in object/edit input methods
			selectedUVs.erase(std::remove_if(selectedUVs.begin(), selectedUVs.end(), [&uvReference, &index](auto a) {return a == uvReference[index[0]]; }), selectedUVs.end());
			selectedUVs.push_back(uvReference[index[0]]);
			std::cout << "\nMULTI SELECT ---> " << index[0];
		}
		else if (index[0] == -1)// normal select .............		-1 is the miss constant
		{
			selectedUVs.clear();
			std::cout << "\n...\n ";
		}
		else
		{
			selectedUVs.clear();
			selectedUVs.push_back(uvReference[index[0]]);
			std::cout << "\nSELECTED ---> " << index[0];


			for (auto x : mesh->getAllFaces())

				for (auto y : x->getLoops())
					if (y->uvVertex == uvReference[index[0]])
						std::cout << "\n Vert the uv belongs to " << mesh->getVertexIndex(y->tip);

		}


		/*
		if (selectMode == SelectMode::EDGE)
		{
			index.clear();

			std::vector<DEdge*> edgesHit;
			EdgeBVHSingleton->getRoot()->Hit(camera->createRay(window), edgesHit);

			for (auto& edge : edgesHit)
			{
				if (!edge)continue;
				auto temp = mesh->getEdgeIndices(edge);
				indexVec.push_back(temp.first);
				indexVec.push_back(temp.second);

			}

			std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();
			DEdge* selectedEdge = nullptr;

			if (indexVec.size())
			{

				// discards all the "hits" beyond the first one.. makes it so the ray "stops after the first hit"

					//std::cout << "\n\n\nVEC SIZE " << indexVec.size();
				index = { indexVec[0],indexVec[1] };

				auto cameraPosition = cameraSingleton->getCamera(0)->getPosition();
				const auto& v = mesh->getVerticesCopy();
				auto v0 = v[indexVec[0]].position;
				auto v1 = v[indexVec[1]].position;
				auto midPoint = glm::vec3((v0.x + v1.x) / 2, (v0.y + v1.y) / 2, (v0.z - v1.z) / 2);
				auto closestPosition = glm::distance(cameraPosition, midPoint);

				for (int i = 2; i < indexVec.size(); i += 2)
				{
					v0 = v[indexVec[i]].position;
					v1 = v[indexVec[i + 1]].position;
					midPoint = glm::vec3(fabs(v0.x - v1.x), fabs(v0.y - v1.y), fabs(v0.z - v1.z));
					auto position = glm::distance(cameraPosition, midPoint);
					if (position < closestPosition)
					{
						closestPosition = position;
						index = { indexVec[i],indexVec[i + 1] };
					}
				}

				selectedEdge = mesh->getEdge(index[0], index[1]);
			}



			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
			{
				if (!selectedEdge)return;

				auto it = std::find(selectedEdges.begin(), selectedEdges.end(), selectedEdge);
				if (it != selectedEdges.end())
					selectedEdges.erase(it);

				selectedEdges.push_back(selectedEdge);

				vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index[0]; }), vertexIndices.end());
				vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [index](int a) {return a == index[1]; }), vertexIndices.end());

				vertexIndices.push_back(index[0]);
				vertexIndices.push_back(index[1]);


				std::cout << "\nMULTI SELECT ---> " << index[0] << " " << index[1];


			}
			else if (!indexVec.size())// normal select ..... miss
			{
				vertexIndices.clear();
				selectedEdges.clear();
			}
			else
			{
				if (!selectedEdge)return;

				vertexIndices.clear();
				selectedEdges.clear();

				vertexIndices.push_back(index[0]);
				vertexIndices.push_back(index[1]);

				selectedEdges.push_back(selectedEdge);

				std::cout << "\nSELECTED ---> " << index[0] << " " << index[1];
			}
		}

		if (selectMode == SelectMode::FACE)
		{
			index.clear();

			std::vector<DFace*>facesHit;
			FaceBVHSingleton->getRoot()->Hit(camera->createRay(window), facesHit);
			std::vector<DFace*>& selectedFaces = mesh->getSelectedFaces();
			int numberOfVerticesInLastFace = 0;

			DFace* selectedFace = nullptr;

			for (auto& face : facesHit)
			{
				if (!face)continue;

				auto temp = mesh->getFaceIndices(face);

				indexVec.push_back(face->getVertices().size());
				indexVec.insert(indexVec.end(), temp.begin(), temp.end());

			}
			/*std::cout << "\n\n ";
			for (auto x : indexVec)
				std::cout << " " << x;
				std::cout << "\n\n ";

			if (indexVec.size()) // if size == 1 it is a miss
			{
				//indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());
				//indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), std::numeric_limits<unsigned int>::max()), indexVec.end());

				std::cout << "\n\n\t indexVec content:\t";
				for (auto x : indexVec)
					std::cout << " " << x;
				std::cout << "\n ";

				// discards all the "hits" beyond the first one.. makes it so the ray "stops after the first hit"

				for (int i = 1;i <= indexVec[0];i++)
					index.push_back(indexVec[i]);


				auto cameraPosition = cameraSingleton->getCamera(0)->getPosition();

				const auto& v = mesh->getVerticesCopy();

				auto midPoint = glm::vec3(0.0f, 0.0f, 0.0f);
				for (int i = 1;i < indexVec[0];i++)
				{
					midPoint = midPoint + v[indexVec[i]].position;
				}
				midPoint = midPoint / float(indexVec[0]);


				auto closestPosition = glm::distance(cameraPosition, midPoint);

				for (int i = 0; i < indexVec.size(); i += indexVec[i] + 1)
				{
					midPoint = glm::vec3(0.0f, 0.0f, 0.0f);
					for (int j = i + 1;j <= i + indexVec[i];j++)
					{
						midPoint = midPoint + v[indexVec[j]].position;

					}
					midPoint = midPoint / float(indexVec[i]);
					auto position = glm::distance(cameraPosition, midPoint);
					if (position < closestPosition)
					{
						closestPosition = position;
						index.clear();
						for (int j = i + 1;j <= i + indexVec[i];j++)
						{
							index.push_back(indexVec[j]);

						};
						numberOfVerticesInLastFace = indexVec[i];
					}
				}

				selectedFace = mesh->getFace(std::unordered_set<int>(index.begin(), index.end()));
			}

			std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();

			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
			{
				if (!selectedFace)return;

				//add the face to selectedFaces atribute
				auto it = std::find(selectedFaces.begin(), selectedFaces.end(), selectedFace);
				if (it != selectedFaces.end())
					selectedFaces.erase(it);
				selectedFaces.push_back(selectedFace);

				for (auto& x : selectedFace->getEdges())
				{
					auto edgeIt = std::find(selectedEdges.begin(), selectedEdges.end(), x);
					if (edgeIt != selectedEdges.end())
						selectedEdges.erase(edgeIt);
					selectedEdges.push_back(x);
				}

				for (auto x : index)
					vertexIndices.erase(std::remove_if(vertexIndices.begin(), vertexIndices.end(), [x](int a) {return a == x; }), vertexIndices.end());

				vertexIndices.insert(vertexIndices.end(), index.begin(), index.end());
				std::cout << "\nMULTI SELECT ---> ";
				for (auto x : index)
					std::cout << x << " ";
			}
			else if (!selectedFace)// miss
			{
				vertexIndices.clear();
				selectedFaces.clear();
				selectedEdges.clear();
			}
			else
			{
				vertexIndices.clear();
				selectedFaces.clear();
				selectedEdges.clear();
				vertexIndices = index;

				selectedFaces.push_back(selectedFace);
				for (auto& x : selectedFace->getEdges())
					selectedEdges.push_back(x);


				std::cout << "\nSELECTED ---> ";
				for (auto x : index)
					std::cout << x << " ";
			}
		}

		*/


		/*	std::cout << "\nSelected vertices ";
		for (auto x : selectedUVs)
			std::cout << x << " ";*/

		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;
	}

	// TRANSLATE
	if (keys[GLFW_KEY_G])
	{

		// Fetches the coordinates of the cursor

		glfwGetCursorPos(glfwWindow, &posX, &posY);

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


		if (keys[GLFW_KEY_X] == 1)
		{
			std::cout << "X";
			for (auto& uv : mesh->getSelectedUVs())
			{
				uv->uv.x += deltaX;
			}
		}
		else if (keys[GLFW_KEY_Y] == 1)
		{
			std::cout << "Y";

			for (auto& uv : mesh->getSelectedUVs())
			{
				uv->uv.y += deltaY;
			}

		}
		else
		{
			for (auto& uv : mesh->getSelectedUVs())
			{
				uv->uv.x += deltaX;
				uv->uv.y += deltaY;
			}

		}

		//mesh->UpdateUVVertexBuffer();
		previousX = posX;
		previousY = posY;
	}

	// for rotate and scale keys.. add them as conditions in the movement2D method's reset if statement 


}
void Application::materialEditor(Window* window)
{

	std::vector<int>& mouseButtons = window->getMouseButtons();
	std::vector<int>& mouseButtonsProcessed = window->getMouseButtonsProcessed();
	std::vector<int>& keys = window->getKeys();
	std::vector<int>& keysProcessed = window->getKeysProcessed();
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();


	GLFWwindow* glfwWindow = window->getGLFWwindow();
	MyGUI& gui = window->getGui();
	static double time = glfwGetTime();



	if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		gui.showAddMenu();
	}

	if (glfwGetKey(glfwWindow, GLFW_KEY_X))
	{

		activeMaterial->deleteSelectedNodes();
		activeMaterial->deleteSelectedLinks();

		keys[GLFW_KEY_X] = 0;
	}

	//if (glfwGetKey(glfwWindow, GLFW_KEY_C))
	//{
	//	if (glfwGetTime() - time < 0.3) return;
	//	time = glfwGetTime();


	//	activeMaterial->compileShader();
	//	keys[GLFW_KEY_C] = 0;
	//}


}
void Application::inputs(Window* window)
{
	GLFWwindow* glfwWindow = window->getGLFWwindow();
	Camera* camera = window->getCamera();
	Mode mode = window->getGui().getMode();

	if (mode == Mode::UV_EDIT)
		camera->movement2D(glfwWindow);
	else
		camera->movement3D(glfwWindow);

	if (mode == Mode::OBJECT)
		Application::objectMode(window);
	else if (mode == Mode::EDIT)
		Application::editMode(window);
	else if (mode == Mode::UV_EDIT)
		Application::uVMode(window);
	else if (mode == Mode::SHADER_EDIT)
	{
		Application::materialEditor(window);
	}
}

void Application::deleteObjects() {}
