#include "Application.h"
#include "CameraManager.h"
#include "MyGUI.h"
#include "DFace.h"
#include "DLoop.h"
#include "GeometryUtils.h"



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
		if (keys[GLFW_KEY_G])
		{
			objectBVHSingleton->Refit();
			keys[GLFW_KEY_G] = 0;

			return;
		}

		std::vector<int> indexVec;
		objectBVHSingleton->getRoot()->Hit(camera->CreateRay(window), indexVec);

		int index = indexVec[0];
		if (indexVec.size() > 1) // if size == 1 it is a miss
		{
			indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());

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

		//std::cout << "\n indices ";
		//for (auto x : objectIndices)
		//	std::cout << x << " ";

		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;

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

	if (keys[GLFW_KEY_TAB])
	{
		keys[GLFW_KEY_TAB] = 0;
		if (!objectIndices.size())
			return;
		mode = Mode::EDIT;
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
	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(objectIndices[objectIndices.size() - 1]));

	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		slide = false;
		firstClick = true;
		if (keys[GLFW_KEY_LEFT_ALT])return;
		keys[GLFW_KEY_S] = 0;
		if (keys[GLFW_KEY_G])
		{
			VertexBVHSingleton->Refit(*mesh);
			EdgeBVHSingleton->Refit(*mesh);
			FaceBVHSingleton->Refit(*mesh);
			keys[GLFW_KEY_G] = 0;
			return;
		}

		std::vector<int>& vertexIndices = mesh->getSelectedVertices();
		std::vector<DVertex*>& verticesReference = mesh->getVertices();
		std::vector<int> indexVec;
		std::vector<int> index = { -1 };

		if (selectMode == SelectMode::VERTEX)
		{
			VertexBVHSingleton->getRoot()->Hit(camera->CreateRay(window), indexVec);

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


			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
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
			EdgeBVHSingleton->getRoot()->Hit(camera->CreateRay(window), edgesHit);

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
			FaceBVHSingleton->getRoot()->Hit(camera->CreateRay(window), facesHit);
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


		std::cout << "\nSelected vertices ";
		for (auto x : vertexIndices)
			std::cout << x << " ";

		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;
		keys[GLFW_KEY_E] = 0;
	}

	// SELECT MODE
	if (keys[GLFW_KEY_1])
	{
		selectMode = SelectMode::VERTEX;
		keys[GLFW_KEY_1] = 0;
	}
	if (keys[GLFW_KEY_2])
	{
		selectMode = SelectMode::EDGE;
		keys[GLFW_KEY_2] = 0;
	}
	if (keys[GLFW_KEY_3])
	{
		selectMode = SelectMode::FACE;
		keys[GLFW_KEY_3] = 0;
	}

	if (keys[GLFW_KEY_TAB])
	{
		keys[GLFW_KEY_TAB] = 0;
		mode = Mode::OBJECT;
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

		/*	glfwGetCursorPos(window, &posX, &posY);

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
				edges[i]->v1->Translate(offset);
				edges[i]->v2->Translate(-offset);
				mesh->UpdateVertexBuffer(mesh->getVertexIndex(edges[i]->v1));
				mesh->UpdateVertexBuffer(mesh->getVertexIndex(edges[i]->v2));
			}
			std::cout << "\nSCALE";
			previousX = posX;
			previousY = posY;*/
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

		std::vector<DVertex*>& vertices = mesh->getVertices();
		if (keys[GLFW_KEY_X] == 1)
		{
			std::cout << "X";
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->Translate(offset.x, 0.0f, 0.0f);
				mesh->UpdateVertexBuffer(x);
			}

			app->updateVertexPosition(glm::vec3(offset.x, 0.0f, 0.0f));
		}
		else if (keys[GLFW_KEY_Y] == 1)
		{
			std::cout << "Y";

			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->Translate(0.0f, offset.y, 0.0f);
				mesh->UpdateVertexBuffer(x);
			}
			app->updateVertexPosition(glm::vec3(0.0f, offset.y, 0.0f));
		}
		else if (keys[GLFW_KEY_Z] == 1)
		{
			std::cout << "Z";
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->Translate(0.0f, 0.0f, offset.z);
				mesh->UpdateVertexBuffer(x);
			}
			app->updateVertexPosition(glm::vec3(0.0f, 0.0f, offset.z));
		}
		else
		{
			for (auto x : mesh->getSelectedVertices())
			{
				vertices[x]->Translate(offset);
				mesh->UpdateVertexBuffer(x);
			}
			app->updateVertexPosition(offset);
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

		glfwGetCursorPos(window, &posX, &posY);


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
							app->updateVertexPosition(deltaX * bestDir);*/
			}
			int j = 0;
			for (j = 0;j < slideDirections[i].size();j++)
			{
				if (glm::all(glm::epsilonEqual(bestDir, slideDirections[i][j], 0.001f)) || glm::all(glm::epsilonEqual(bestDir, -slideDirections[i][j], 0.001f)))
					break;
			}

			tempVertices[selectedVertices[i]]->Translate(fabs(deltaX) * bestDir);


			float projectedLength = glm::dot(tempVertices[selectedVertices[i]]->position - startPositions[i], bestDir); // scalar movement along direction
			float maxLength = glm::length(slideLengths[i][j] - startPositions[i]);
			projectedLength = glm::clamp(projectedLength, 0.0f, maxLength);

			tempVertices[selectedVertices[i]]->position = startPositions[i] + bestDir * projectedLength;

			mesh->UpdateVertexBuffer(selectedVertices[i]);

			if (i == selectedVertices.size() - 1)
				app->updateVertexPosition(bestDir * projectedLength);
		}


		previousX = posX;
		previousY = posY;
	}

	// INSET
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		gui.InsetMenu();

		keys[GLFW_KEY_I] = 0;
		//	keys[GLFW_KEY_S] = 1;
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
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{

		gui.DeleteMenu();

	}

	// EXTRUDE
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {

		gui.ExtrudeMenu();

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

	// SIMPLE SUBDIVISION
	if (keys[GLFW_KEY_S] && keys[GLFW_KEY_LEFT_CONTROL])
	{
		mesh->linearSubdivision();
		keys[GLFW_KEY_S] = 0;
		keys[GLFW_KEY_LEFT_CONTROL] = 0;
	}

	// LOOP CUT
	if (keys[GLFW_KEY_R] && keys[GLFW_KEY_LEFT_CONTROL])
	{
		mesh->loopCut(mesh->getSelectedEdges().back(), 3);
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
void Application::UVMode(GLFWwindow* window, MyGUI& gui)
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

	static Camera* camera = cameraSingleton->getCamera("UV");
	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(objectIndices.back()));

	// SELECT
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
	{
		firstClick = true;

		if (keys[GLFW_KEY_G])
		{
			UVVertexBVHSingleton->Refit(*mesh);
			//EdgeBVHSingleton->Refit(*mesh);
			//FaceBVHSingleton->Refit(*mesh);
			keys[GLFW_KEY_G] = 0;
			return;
		}

		auto& vertexIndicesss = mesh->getSelectedUVs();
		auto& selectedUVs = mesh->getSelectedUVs();
		auto& uvReference = mesh->getUVCoords();
		std::vector<int> indexVec;
		std::vector<int> index = { -1 };


		UVVertexBVHSingleton->getRoot()->Hit(camera->CreateRay(window), indexVec);



		if (indexVec.size() > 1) // if size == 1 it is a miss
		{
			indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), -1), indexVec.end());
			indexVec.erase(std::remove(indexVec.begin(), indexVec.end(), std::numeric_limits<unsigned int>::max()), indexVec.end());

			if (indexVec.size())
				index[0] = indexVec[0];
			else index[0] = -1;
		}


		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // shift select
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


			for(auto x: mesh->getAllFaces())

				for(auto y:x->getLoops())
					if (y->uvVertex == uvReference[index[0]])
						std::cout << "\n Vert the uv belongs to " << mesh->getVertexIndex(y->tip);

		}


		/*
		if (selectMode == SelectMode::EDGE)
		{
			index.clear();

			std::vector<DEdge*> edgesHit;
			EdgeBVHSingleton->getRoot()->Hit(camera->CreateRay(window), edgesHit);

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
			FaceBVHSingleton->getRoot()->Hit(camera->CreateRay(window), facesHit);
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
		if (!mesh->getSelectedVertices().size())
		{
			keys[GLFW_KEY_G] = 0;
			return;
		}
		// Fetches the coordinates of the cursor

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

		auto& vertices = mesh->getUVCoords();
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

}
void Application::Inputs(GLFWwindow* window, MyGUI& gui)
{
	Window* classWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	Camera* camera = classWindow->getCamera();

	Mode mode = gui.getMode();

	if (mode == Mode::UV_EDITOR)
		camera->Movement2D(window, gui);
	else
		camera->Movement3D(window, gui);

	if (mode == Mode::OBJECT)
		Application::ObjectMode(window, gui);
	else if (mode == Mode::EDIT)
		Application::EditMode(window, gui);
	else if (mode == Mode::UV_EDITOR)
		Application::UVMode(window, gui);
}

void Application::deleteObjects() {}
