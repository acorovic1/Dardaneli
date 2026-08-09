#include "Viewport.h"
#include "Window.h"

int Viewport::nextViewportId = 0;

Viewport::Viewport(double left, double bottom, double right, double top, GLFWwindow* glfwWindow) :
	left(left), right(right), bottom(bottom), top(top)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

	int width = window->getWidth();
	int height = window->getHeight();

	cameras[(size_t)CameraTypes::VIEWPORT] = std::make_unique<Camera>((right - left) * width, (top - bottom) * height, glm::vec3(-2.0f, 3.0f, 5.0f), "Viewport");
	cameras[(size_t)CameraTypes::UV] = std::make_unique<Camera>((right - left) * width, (top - bottom) * height, glm::vec3(0.5f, 0.5f, 1.0f), "UV");
	cameras[(size_t)CameraTypes::SHADER] = std::make_unique<Camera>((right - left) * width, (top - bottom) * height, glm::vec3(0.0f, 0.0f, 6.0f), "Shader");

	float aspect = ((right - left) * width) / ((top - bottom) * height);
	cameras[(size_t)CameraTypes::VIEWPORT]->setPerspectiveProjection(45, aspect, 0.1f, 10000.0f);
	cameras[(size_t)CameraTypes::UV]->setOrthographicProjection(1.0f, aspect, -2000.0f, 300000.0f);

	cameras[(size_t)CameraTypes::UV]->setOrientation(glm::vec3(0.0f, 0.0f, -1.0f));


	glViewport(left * width, bottom * height, right * width, top * height);

	guiId = nextViewportId++;
	//dockspaceId = ImGui::GetID(("vp_dock_" + std::to_string(id)).c_str());
	//windowClass.ClassId = ImGui::GetID(("vp_class_" + std::to_string(id)).c_str());
	//windowClass.DockingAllowUnclassed = false;

}

Viewport::Viewport(Viewport* baseViewport, double xPos, double yPos, bool vertical, Window* window)
{

	int width = window->getWidth();
	int height = window->getHeight();


	if (vertical)
	{
		left = baseViewport->getLeft();
		right = xPos / width;
		bottom = baseViewport->getBottom();
		top = baseViewport->getTop();


	}
	else
	{
		left = baseViewport->getLeft();
		right = baseViewport->getRight();
		bottom = baseViewport->getBottom();
		top = (height - yPos) / height;


	}




	//std::cout << "\nright = " << right << "\nleft = " << left << "\ntop = " << top << "\nbottom = "
	//	<< bottom << "\nwidth = " << (right - left) * width << "\nheight = " << (top - bottom) * height;

	Camera* baseViewportCamera = baseViewport->getCamera(CameraTypes::VIEWPORT);
	cameras[(size_t)CameraTypes::VIEWPORT] = std::make_unique<Camera>((right - left) * width, (top - bottom) * height,
		baseViewportCamera->getPosition(), baseViewportCamera->getOrientation(), baseViewportCamera->getUp(), "Viewport");

	baseViewportCamera = baseViewport->getCamera(CameraTypes::UV);
	cameras[(size_t)CameraTypes::UV] = std::make_unique<Camera>((right - left) * width, (top - bottom) * height,
		baseViewportCamera->getPosition(), baseViewportCamera->getOrientation(), baseViewportCamera->getUp(), "UV");

	baseViewportCamera = baseViewport->getCamera(CameraTypes::SHADER);
	cameras[(size_t)CameraTypes::SHADER] = std::make_unique<Camera>((right - left) * width, (top - bottom) * height,
		baseViewportCamera->getPosition(), baseViewportCamera->getOrientation(), baseViewportCamera->getUp(), "Shader");


	float aspect = ((right - left) * width) / ((top - bottom) * height);
	cameras[(size_t)CameraTypes::VIEWPORT]->setPerspectiveProjection(45, aspect, 0.1f, 10000.0f);
	cameras[(size_t)CameraTypes::UV]->setOrthographicProjection(1.0f, aspect, -2000.0f, 300000.0f);

	cameras[(size_t)CameraTypes::UV]->setOrientation(glm::vec3(0.0f, 0.0f, -1.0f));


	if (vertical)
		baseViewport->adjustSize(right, bottom, baseViewport->getRight(), top, window);
	else
		baseViewport->adjustSize(left, top, right, baseViewport->getTop(), window);



	glViewport(left * width, bottom * height, right * width, top * height);


	guiId = nextViewportId++;
	//dockspaceId = ImGui::GetID(("vp_dock_" + std::to_string(id)).c_str());
	//windowClass.ClassId = ImGui::GetID(("vp_class_" + std::to_string(id)).c_str());
	//windowClass.DockingAllowUnclassed = false;
}

Viewport::~Viewport()
{

}






void Viewport::drawGui(MyGUI& gui)
{
	glm::ivec4 corners = this->getCorners(gui.getWindow());
	int windowHeight = gui.getWindow()->getHeight();

	float posX = static_cast<float>(corners.x);
	float posY = static_cast<float>(windowHeight - corners.w);
	float width = static_cast<float>(corners.z - corners.x);
	float height = static_cast<float>(corners.w - corners.y);


	if (corners.w == windowHeight)
	{
		posY = 0.051 * height;
		height -= posY;
	}

	if (width <= 0.0f || height <= 0.0f) return;


	ImGui::SetNextWindowPos(ImVec2(posX, posY));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	std::string hostName = "PaneHost##" + std::to_string(guiId);
	ImGui::Begin(hostName.c_str(), nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoDocking);

	ImGuiID dockspaceId = ImGui::GetID("ViewportDockSpace");

	if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr)
	{
		ImGui::DockBuilderRemoveNode(dockspaceId);
		ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceId, ImVec2(width, height));

		ImGuiID dock_main_id = dockspaceId;

		ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, nullptr, &dock_main_id);
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.15f, nullptr, &dock_main_id);

		//ImGuiID dock_id_shader = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_, 0.35f, nullptr, &dock_main_id);


		ImGui::DockBuilderDockWindow(("Shader Node Editor##" + std::to_string(guiId)).c_str(), dock_main_id);

		// Assign windows to their respective split nodes
		//ImGui::DockBuilderDockWindow(("RenderMode##" + std::to_string(id)).c_str(), dock_id_top);
		ImGui::DockBuilderDockWindow(("Editor Tools##" + std::to_string(guiId)).c_str(), dock_id_left);
		ImGui::DockBuilderDockWindow(("Hierarchy##" + std::to_string(guiId)).c_str(), dock_id_right);

		ImGui::DockBuilderFinish(dockspaceId);
	}

	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::SetNextWindowSizeConstraints(ImVec2(5.0f, 5.0f), ImVec2(FLT_MAX, FLT_MAX));

	ImGui::Begin(("Editor Tools##" + std::to_string(guiId)).c_str());
	switch (mode)
	{
	case Mode::OBJECT:      gui.drawObjectTools(this);		break;
	case Mode::EDIT:        gui.drawEditTools(this);		break;
	case Mode::UV_EDIT:     gui.drawUVTools(this);			break;
	case Mode::SHADER_EDIT: gui.drawNodeTools(this);	break;
	}
	ImGui::End();

	switch (mode)
	{
		//case Mode::OBJECT:      gui.drawObjectModeUI(this);		break;
		//case Mode::EDIT:        gui.drawEditModeUI(this);		break;
		//case Mode::UV_EDIT:     gui.drawUVModeUI(this);			break;
	case Mode::SHADER_EDIT: gui.shaderNodeEditor(this);	break;
	}



}

bool Viewport::cursorWrapAround(Window* window, double& posX, double& posY)
{
	double startX = left * window->getWidth();
	double startY = bottom * window->getHeight();
	double endX = right * window->getWidth();
	double endY = top * window->getHeight();

	bool wrapped = false;

	if (posX >= endX)
	{
		posX = startX + 1;
		wrapped = true;
	}
	else if (posX <= startX)
	{
		posX = endX - 1;
		wrapped = true;
	}

	if (posY >= endY)
	{
		posY = startY + 1;
		wrapped = true;
	}
	else if (posY <= startY)
	{
		posY = endY - 1;
		wrapped = true;
	}

	if (wrapped)
		glfwSetCursorPos(window->getGLFWwindow(), posX, posY);

	return wrapped;
}

std::set<Viewport*> Viewport::getViewportsAtOppositeSideFromBorder(Window* window, ViewportBoundary boundary)
{
	std::set<Viewport*> returnSet;

	auto& viewports = window->getViewports();

	if (boundary == ViewportBoundary::BOTTOM)
	{
		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (fabs(bottom - viewports[i]->top) < myEpsilon)
				returnSet.insert(viewports[i].get());
		}
	}
	else if (boundary == ViewportBoundary::TOP)

	{
		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (fabs(top - viewports[i]->bottom) < myEpsilon)
				returnSet.insert(viewports[i].get());

		}
	}
	else if (boundary == ViewportBoundary::LEFT)
	{

		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (fabs(left - viewports[i]->right) < myEpsilon)
				returnSet.insert(viewports[i].get());

		}
	}
	else if (boundary == ViewportBoundary::RIGHT)
	{
		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (fabs(right - viewports[i]->left) < myEpsilon)
				returnSet.insert(viewports[i].get());
		}
	}

	return returnSet;
}


std::set<Viewport*> Viewport::getAdjecentViewports(Window* window, ViewportBoundary boundary)
{
	std::set<Viewport*> returnSet;

	auto& viewports = window->getViewports();

	if (boundary == ViewportBoundary::BOTTOM)
	{
		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (bottom == viewports[i]->top)
				if (left <= viewports[i]->right && right >= viewports[i]->left)
					returnSet.insert(viewports[i].get());
		}
	}
	else if (boundary == ViewportBoundary::TOP)
	{
		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (top == viewports[i]->bottom)
				if (left <= viewports[i]->right && right >= viewports[i]->left)
					returnSet.insert(viewports[i].get());
		}
	}
	else if (boundary == ViewportBoundary::LEFT)
	{

		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (left == viewports[i]->right)
				if (bottom <= viewports[i]->top && top >= viewports[i]->bottom) //  &&  (bottom != viewports[i]->top || top != viewports[i]->bottom) ako bude trebao cosak istrijebiti tj da ne broji onaj po dijagonali

					returnSet.insert(viewports[i].get());
		}
	}
	else if (boundary == ViewportBoundary::RIGHT)
	{
		for (int i = 0;i < viewports.size();i++)
		{
			if (this == viewports[i].get())
				continue;
			if (right == viewports[i]->left)
				if (bottom <= viewports[i]->top && top >= viewports[i]->bottom)
					returnSet.insert(viewports[i].get());
		}
	}

	return returnSet;
}



double& Viewport::edge(Viewport* vp, ViewportBoundary b)
{
	switch (b)
	{
	case ViewportBoundary::TOP:    return vp->top;
	case ViewportBoundary::BOTTOM: return vp->bottom;
	case ViewportBoundary::LEFT:   return vp->left;
	case ViewportBoundary::RIGHT:  return vp->right;
	}
	throw std::invalid_argument("edge: bad boundary");
}

ViewportBoundary Viewport::opposite(ViewportBoundary b)
{
	switch (b)
	{
	case ViewportBoundary::TOP:    return ViewportBoundary::BOTTOM;
	case ViewportBoundary::BOTTOM: return ViewportBoundary::TOP;
	case ViewportBoundary::LEFT:   return ViewportBoundary::RIGHT;
	case ViewportBoundary::RIGHT:  return ViewportBoundary::LEFT;
	default: return ViewportBoundary::NONE;
	}
}

bool Viewport::exceeds(Viewport* vp, ViewportBoundary b, double bound)
{
	switch (b)
	{
	case ViewportBoundary::RIGHT:  return vp->right > bound;
	case ViewportBoundary::LEFT:   return vp->left < bound;
	case ViewportBoundary::TOP:    return vp->top > bound;
	case ViewportBoundary::BOTTOM: return vp->bottom < bound;
	default: return false;
	}
}

void Viewport::resolveSideOverhang(Window* window, Viewport* neighborVp, Viewport* boundEdgeVp,
	ViewportBoundary sideBoundary, ViewportBoundary pushBoundary,
	ViewportBoundary rangeNearB, ViewportBoundary rangeFarB,
	double rangeNear, double rangeFar)
{
	if (!exceeds(neighborVp, sideBoundary, edge(boundEdgeVp, sideBoundary)))
		return;

	std::set<Viewport*> vpSide = neighborVp->getAdjecentViewports(window, sideBoundary);
	bool moveSide = !vpSide.empty();
	for (auto sideVp : vpSide)
	{
		if (edge(sideVp, rangeNearB) > rangeNear || edge(sideVp, rangeFarB) < rangeFar)
		{
			moveSide = false;
			break;
		}
	}

	if (moveSide)
	{
		for (auto temp : vpSide)
		{
			edge(temp, opposite(sideBoundary)) = edge(boundEdgeVp, sideBoundary);
			temp->adjustSize(window);
		}
		std::cout << "\n\t moved VPside (opposite " << (int)sideBoundary << ")!";
	}
	else
	{
		for (auto pushVp : neighborVp->getAdjecentViewports(window, pushBoundary))
		{
			if (pushVp == boundEdgeVp)
				continue;
			edge(pushVp, opposite(pushBoundary)) = edge(neighborVp, opposite(pushBoundary));
			pushVp->adjustSize(window);
			std::cout << "\n\t moved VP-side (pushed along " << (int)pushBoundary << ")!";
		}
	}

	edge(neighborVp, sideBoundary) = edge(boundEdgeVp, sideBoundary);
}

void Viewport::resolveAxisOverhang(Window* window, Viewport* vp, Viewport* viewportMin,
	ViewportBoundary axisBoundary, ViewportBoundary fallbackSide,
	ViewportBoundary rangeNearB, ViewportBoundary rangeFarB,
	double rangeNear, double rangeFar)
{
	if (!exceeds(vp, axisBoundary, edge(viewportMin, axisBoundary)))
		return;

	ViewportBoundary pushBoundary = opposite(axisBoundary);
	auto neighbors = vp->getAdjecentViewports(window, axisBoundary);

	if (!neighbors.empty())
	{
		ViewportBoundary p1, p2;
		perpendicularPairOf(axisBoundary, p1, p2);

		for (auto neighborVp : neighbors)
		{
			std::cout << "\n\t\t\t Hello!";
			resolveSideOverhang(window, neighborVp, vp, p1, pushBoundary, rangeNearB, rangeFarB, rangeNear, rangeFar);
			resolveSideOverhang(window, neighborVp, vp, p2, pushBoundary, rangeNearB, rangeFarB, rangeNear, rangeFar);

			edge(neighborVp, opposite(axisBoundary)) = edge(viewportMin, axisBoundary);
			neighborVp->adjustSize(window);
		}
	}
	else
	{
		for (auto vpSide : vp->getAdjecentViewports(window, fallbackSide))
		{
			if (exceeds(vpSide, axisBoundary, edge(viewportMin, axisBoundary)))
			{
				edge(vpSide, opposite(fallbackSide)) = edge(vp, opposite(fallbackSide));
				vpSide->adjustSize(window);
				std::cout << "\n\t idk what to name this one tbh!!";
			}
		}
	}

	edge(vp, axisBoundary) = edge(viewportMin, axisBoundary);
}

void Viewport::clampToViewportMin(Window* window, Viewport* viewportMin,
	ViewportBoundary sideBoundary, ViewportBoundary primaryBoundary,
	bool& clampFlag, bool& moveSideUpFlag,
	ViewportBoundary rangeNearB, ViewportBoundary rangeFarB,
	double rangeNear, double rangeFar)
{
	if (!exceeds(viewportMin, sideBoundary, edge(this, sideBoundary)))
		return;

	std::cout << "\n\t clamped (side " << (int)sideBoundary << ")!";
	clampFlag = true;

	ViewportBoundary towardsThis = opposite(primaryBoundary);

	if (viewportMin->getAdjecentViewports(window, sideBoundary).empty())
	{
		moveSideUpFlag = true;
		for (auto vp : viewportMin->getAdjecentViewports(window, towardsThis))
		{
			if (vp == this) continue;
			if (exceeds(vp, sideBoundary, edge(this, sideBoundary)))
			{
				edge(vp, primaryBoundary) = edge(viewportMin, primaryBoundary);
				vp->adjustSize(window);
				std::cout << "\n\t side moved up!";
			}
		}
		return;
	}

	ViewportBoundary fallbackSide = opposite(sideBoundary);
	ViewportBoundary a1, a2;
	perpendicularPairOf(sideBoundary, a1, a2);

	for (auto vp : viewportMin->getAdjecentViewports(window, sideBoundary))
	{
		resolveAxisOverhang(window, vp, viewportMin, a1, fallbackSide, rangeNearB, rangeFarB, rangeNear, rangeFar);
		resolveAxisOverhang(window, vp, viewportMin, a2, fallbackSide, rangeNearB, rangeFarB, rangeNear, rangeFar);

		edge(vp, opposite(sideBoundary)) = edge(this, sideBoundary);
		vp->adjustSize(window);
	}
}



void Viewport::joinViewport(Window* window, ViewportBoundary boundary)
{
	if (boundary != ViewportBoundary::NONE)
	{
		std::set<Viewport*> viewportsPrimary = this->getAdjecentViewports(window, boundary);

		if (!viewportsPrimary.size())
		{
			window->deleteDegenerateViewports();
			this->adjustSize(window);
			return;
		}

		Viewport* viewportMin{ *viewportsPrimary.begin() };

		for (auto viewport : viewportsPrimary)
			if (isCloserToThis(viewport, viewportMin, boundary))
				viewportMin = viewport;

		ViewportBoundary rangeNearB, rangeFarB;
		rangeAxisOf(boundary, rangeNearB, rangeFarB);
		double rangeNear = edge(this, rangeNearB);
		double rangeFar = edge(this, rangeFarB);

		ViewportBoundary p1, p2; // the perpendicular axis pair, e.g. {LEFT,RIGHT} when boundary is TOP/BOTTOM
		perpendicularPairOf(boundary, p1, p2);

		bool clampP1 = false, clampP2 = false;
		bool moveP1Up = false, moveP2Up = false;

		clampToViewportMin(window, viewportMin, p1, boundary, clampP1, moveP1Up, rangeNearB, rangeFarB, rangeNear, rangeFar);
		clampToViewportMin(window, viewportMin, p2, boundary, clampP2, moveP2Up, rangeNearB, rangeFarB, rangeNear, rangeFar);

		// Can the side viewports (along p1/p2) move freely, i.e. do they all fit within this's range?
		std::set<Viewport*> viewportsP1 = this->getAdjecentViewports(window, p1);
		bool moveP1 = viewportsP1.size() ? true : false;
		for (auto vp : viewportsP1)
		{
			if (edge(vp, rangeNearB) > rangeNear || edge(vp, rangeFarB) < rangeFar)
			{
				moveP1 = false;
				break;
			}
		}

		std::set<Viewport*> viewportsP2 = this->getAdjecentViewports(window, p2);
		bool moveP2 = viewportsP2.size() ? true : false;
		for (auto vp : viewportsP2)
		{
			if (edge(vp, rangeNearB) > rangeNear || edge(vp, rangeFarB) < rangeFar)
			{
				moveP2 = false;
				break;
			}
		}

		if (moveP1 && !clampP1)
		{
			for (auto vp : getAdjecentViewports(window, p1))
			{
				edge(vp, opposite(p1)) = edge(viewportMin, p1);
				vp->adjustSize(window);
				std::cout << "\n\t moved along p1!";
			}
		}
		else if (!clampP1)
		{
			for (auto vp : viewportsPrimary)
			{
				if (vp == viewportMin)
					continue;
				if (exceeds(vp, p1, edge(viewportMin, p1)))
				{
					edge(vp, opposite(boundary)) = edge(this, opposite(boundary));
					vp->adjustSize(window);
					std::cout << "\n\t NOT moved along p1... but pushed along primary!";
				}
			}
		}

		if (moveP2 && !clampP2)
		{
			for (auto vp : getAdjecentViewports(window, p2))
			{
				edge(vp, opposite(p2)) = edge(viewportMin, p2);
				vp->adjustSize(window);
				std::cout << "\n\t moved along p2!";
			}
		}
		else if (!clampP2)
		{
			for (auto vp : viewportsPrimary)
			{
				if (vp == viewportMin)
					continue;
				if (exceeds(vp, p2, edge(viewportMin, p2)))
				{
					edge(vp, opposite(boundary)) = edge(this, opposite(boundary));
					vp->adjustSize(window);
					std::cout << "\n\t NOT moved along p2... but pushed along primary!";
				}
			}
		}

		if (clampP1)
			edge(viewportMin, p1) = edge(this, p1);
		if (clampP2)
			edge(viewportMin, p2) = edge(this, p2);

		edge(this, p1) = edge(viewportMin, p1);
		edge(this, p2) = edge(viewportMin, p2);
		edge(this, boundary) = edge(viewportMin, boundary);

		window->deleteViewport(viewportMin);

		window->deleteDegenerateViewports();
		this->adjustSize(window);

		return;
	}

	window->deleteDegenerateViewports();
	this->adjustSize(window);
}

glm::ivec4 Viewport::getCorners(Window* window)
{

	int width = window->getWidth();
	int height = window->getHeight();
	glm::ivec4 corners;

	corners.x = left * width;
	corners.y = bottom * height;
	corners.z = right * width;
	corners.w = top * height;

	return corners;
}

void Viewport::resize(Window* window, ViewportBoundary boundary)
{
	bool& firstClick = window->getFirstClick();
	double& posX = window->getPosX();
	double& posY = window->getPosY();
	double& previousX = window->getPreviousX();
	double& previousY = window->getPreviousY();

	double winHeight = window->getHeight();
	double winWidth = window->getWidth();

	glfwGetCursorPos(window->getGLFWwindow(), &posX, &posY);
	if (posX >= winWidth || posX <= 0 || posY <= 0 || posY >= winHeight)
		return;

	if (firstClick)
	{
		previousX = posX;
		previousY = posY;
		firstClick = false;
	}
	else
	{
		auto& viewports = window->getViewports();

		double offsetY = (previousY - posY) / winHeight;
		double offsetX = (posX - previousX) / winWidth;

		std::set<int> change1;
		std::set<int> change2;

		if (boundary == ViewportBoundary::LEFT)
		{
			for (int i = 0;i < viewports.size();i++)
			{
				if (viewports[i].get() == this)
					continue;

				if (this->left == viewports[i]->getRight())
					change1.insert(i);

			}

			for (int i = 0;i < viewports.size();i++)
			{

				for (int j : change1)
				{
					if (viewports[i] == viewports[j])
						continue;

					if (viewports[i]->getLeft() == viewports[j]->getRight())
						change2.insert(i);


				}
			}

			for (int i : change1)
			{
				viewports[i]->setRight(viewports[i]->getRight() + offsetX);
				viewports[i]->adjustSize(window);

			}
			for (int i : change2)
			{
				viewports[i]->setLeft(viewports[i]->getLeft() + offsetX);
				viewports[i]->adjustSize(window);
			}

		}
		else if (boundary == ViewportBoundary::BOTTOM)
		{
			for (int i = 0;i < viewports.size();i++)
			{
				if (viewports[i].get() == this)
					continue;

				if (this->bottom == viewports[i]->getTop())
					change1.insert(i);

			}

			for (int i = 0;i < viewports.size();i++)
			{

				for (int j : change1)
				{
					if (viewports[i] == viewports[j])
						continue;

					if (viewports[i]->getBottom() == viewports[j]->getTop())
						change2.insert(i);


				}
			}

			for (int i : change1)
			{
				viewports[i]->setTop(viewports[i]->getTop() + offsetY);
				viewports[i]->adjustSize(window);
			}
			for (int i : change2)
			{
				viewports[i]->setBottom(viewports[i]->getBottom() + offsetY);
				viewports[i]->adjustSize(window);
			}
		}
		else if (boundary == ViewportBoundary::RIGHT)
		{
			for (int i = 0;i < viewports.size();i++)
			{
				if (viewports[i].get() == this)
					continue;

				if (this->right == viewports[i]->getLeft())
					change1.insert(i);

			}

			for (int i = 0;i < viewports.size();i++)
			{

				for (int j : change1)
				{
					if (viewports[i] == viewports[j])
						continue;

					if (viewports[i]->getRight() == viewports[j]->getLeft())
						change2.insert(i);


				}
			}

			for (int i : change1)
			{
				viewports[i]->setLeft(viewports[i]->getLeft() + offsetX);
				viewports[i]->adjustSize(window);

			}
			for (int i : change2)
			{
				viewports[i]->setRight(viewports[i]->getRight() + offsetX);
				viewports[i]->adjustSize(window);
			}
		}
		else if (boundary == ViewportBoundary::TOP)
		{

			for (int i = 0;i < viewports.size();i++)
			{
				if (viewports[i].get() == this)
					continue;

				if (this->top == viewports[i]->getBottom())
					change1.insert(i);

			}

			for (int i = 0;i < viewports.size();i++)
			{

				for (int j : change1)
				{
					if (viewports[i] == viewports[j])
						continue;

					if (viewports[i]->getTop() == viewports[j]->getBottom())
						change2.insert(i);


				}
			}

			for (int i : change1)
			{
				viewports[i]->setBottom(viewports[i]->getBottom() + offsetY);
				viewports[i]->adjustSize(window);
			}
			for (int i : change2)
			{
				viewports[i]->setTop(viewports[i]->getTop() + offsetY);
				viewports[i]->adjustSize(window);
			}

		}

		previousY = posY;
		previousX = posX;

	}

}

void Viewport::resizeWindow(int windowWidth, int windowHeight)
{

	int width = (right - left) * windowWidth;
	int height = (top - bottom) * windowHeight;

	for (int i = 0;i < cameras.size();i++)
	{
		cameras[i]->setWidth(width);
		cameras[i]->setHeight(height);
	}

	float aspect = 0;
	if (height != 0)
		aspect = float(width) / float(height);
	cameras[(size_t)CameraTypes::VIEWPORT]->setPerspectiveProjection(45, aspect, 0.1f, 10000.0f);
	cameras[(size_t)CameraTypes::UV]->setOrthographicProjection(1.0f, aspect, -2000.0f, 300000.0f);

	glViewport(left * width, bottom * height, right * width, top * height);
}

void Viewport::adjustSize(double newLeft, double newBottom, double newRight, double newTop, Window* window)
{
	left = newLeft;
	bottom = newBottom;
	right = newRight;
	top = newTop;




	int width = window->getWidth();
	int height = window->getHeight();
	for (int i = 0;i < cameras.size();i++)
	{
		cameras[i]->setWidth((right - left) * width);
		cameras[i]->setHeight((top - bottom) * height);
	}

	float aspect = ((right - left) * width) / ((top - bottom) * height);
	cameras[(size_t)CameraTypes::VIEWPORT]->setPerspectiveProjection(45, aspect, 0.1f, 10000.0f);
	cameras[(size_t)CameraTypes::UV]->setOrthographicProjection(1.0f, aspect, -2000.0f, 300000.0f);


	//std::cout << "\n\n\tAdjusted size:\n";
	//std::cout << "\nright = " << right << "\nleft = " << left << "\ntop = " << top << "\nbottom = "
	//	<< bottom << "\nwidth = " << width << "\nheight = " << height;

	glViewport(left * width, bottom * height, right * width, top * height);
}
















