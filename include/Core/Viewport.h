#pragma once

#include <vector>
#include <array>
#include <set>
#include <memory>

#include "Camera.h"
#include "CameraManager.h"
#include "Enums.h"
#include "MyGUI.h"

class Viewport {

	double left, right, bottom, top;



	std::array<std::unique_ptr<Camera>, (size_t)CameraTypes::COUNT> cameras;
	int currentCameraIndex = (size_t)CameraTypes::VIEWPORT;

	Mode mode = Mode::OBJECT;
	SelectMode selectMode = SelectMode::VERTEX;
	RenderMode renderMode = RenderMode::SOLID;




	std::vector<int>keys = std::vector<int>(1024, 0);      // Holds current state (pressed or not)
	std::vector<int>keysProcessed = std::vector<int>(1024, 0);    // Ensures action happens once per press
	std::vector<int>mouseButtons = std::vector<int>(3, 0);
	std::vector<int>mouseButtonsProcessed = std::vector<int>(3, 0);



	static double& edge(Viewport* vp, ViewportBoundary b);
	static bool exceeds(Viewport* vp, ViewportBoundary b, double bound);
	static ViewportBoundary opposite(ViewportBoundary b);

	void resolveSideOverhang(Window* window, Viewport* neighborVp, Viewport* boundEdgeVp,
		ViewportBoundary sideBoundary, ViewportBoundary pushBoundary,
		ViewportBoundary rangeNearB, ViewportBoundary rangeFarB,
		double rangeNear, double rangeFar);
	void resolveAxisOverhang(Window* window, Viewport* vp, Viewport* viewportMin,
		ViewportBoundary axisBoundary, ViewportBoundary fallbackSide,
		ViewportBoundary rangeNearB, ViewportBoundary rangeFarB,
		double rangeNear, double rangeFar);
	void clampToViewportMin(Window* window, Viewport* viewportMin,
		ViewportBoundary sideBoundary, ViewportBoundary primaryBoundary,
		bool& clampFlag, bool& moveSideUpFlag,
		ViewportBoundary rangeNearB, ViewportBoundary rangeFarB,
		double rangeNear, double rangeFar);



	// The pair {TOP,BOTTOM} or {LEFT,RIGHT} perpendicular to b's own axis.
	static void perpendicularPairOf(ViewportBoundary b, ViewportBoundary& p1, ViewportBoundary& p2)
	{
		if (b == ViewportBoundary::TOP || b == ViewportBoundary::BOTTOM)
		{
			p1 = ViewportBoundary::LEFT;
			p2 = ViewportBoundary::RIGHT;
		}
		else
		{
			p1 = ViewportBoundary::TOP;
			p2 = ViewportBoundary::BOTTOM;
		}
	}

	// Within b's own axis pair, which member uses ">" ("near"/grows-positive) vs "<" ("far") in exceeds().
	static void rangeAxisOf(ViewportBoundary b, ViewportBoundary& nearB, ViewportBoundary& farB)
	{
		if (b == ViewportBoundary::TOP || b == ViewportBoundary::BOTTOM)
		{
			nearB = ViewportBoundary::TOP;
			farB = ViewportBoundary::BOTTOM;
		}
		else
		{
			nearB = ViewportBoundary::RIGHT;
			farB = ViewportBoundary::LEFT;
		}
	}

	// Should candidate replace currentMin as the "innermost" adjacent viewport along boundary?
	static bool isCloserToThis(Viewport* candidate, Viewport* currentMin, ViewportBoundary b)
	{
		switch (b)
		{
		case ViewportBoundary::TOP:    return candidate->top < currentMin->top;
		case ViewportBoundary::BOTTOM: return candidate->bottom > currentMin->bottom;
		case ViewportBoundary::LEFT:  return candidate->left > currentMin->left;
		case ViewportBoundary::RIGHT:  return candidate->right < currentMin->right;
		default: return false;
		}
	}


public:

	Viewport() = delete;
	Viewport(const Viewport& copy) = delete;
	Viewport(Viewport&& move) = delete;
	Viewport& operator=(const Viewport& copy) = delete;
	Viewport& operator=(Viewport&& move) = delete;


	
	Viewport(double left, double bottom, double right, double top, GLFWwindow* glfwWindow);
	// used for adding aditional viewports
	Viewport(Viewport* baseViewport, double xPos, double yPos, bool vertical, Window* window);
	~Viewport();

	double getLeft() const { return left; }
	double getRight() const { return right; }
	double getBottom() const { return bottom; }
	double getTop() const { return top; }

	void setLeft(double v) { left = v; }
	void setBottom(double v) { bottom = v; }
	void setRight(double v) { right = v; }
	void setTop(double v) { top = v; }


	Camera* getCamera(CameraTypes type) { return cameras[(size_t)type].get(); }
	Camera* getActiveCamera() { return cameras[currentCameraIndex].get(); }
	void setActiveCamera(CameraTypes type) { currentCameraIndex = (size_t)type; }

	Mode& getMode() { return mode; }
	SelectMode& getSelectMode() { return selectMode; }
	RenderMode& getRenderMode() { return renderMode; }

	void setMode(Mode newMode) { mode = newMode; }
	void setRenderMode(RenderMode newRenderMode) { renderMode = newRenderMode; }


	std::vector<int>& getKeys() { return keys; }
	std::vector<int>& getKeysProcessed() { return keysProcessed; }
	std::vector<int>& getMouseButtons() { return mouseButtons; }
	std::vector<int>& getMouseButtonsProcessed() { return mouseButtonsProcessed; }


	bool cursorWrapAround(Window* window, double& posX, double& posY);



	// 
	// Returns all viewports whose opposite border is aligned with the specified boundary of this viewport.
	// e.g left = vp.right is valid.. left = vp.left is NOT valid
	// The returned set excludes this viewport itself.
	std::set<Viewport*> getViewportsAtOppositeSideFromBorder(Window* window, ViewportBoundary boundary);
	//  treba napraviti jos jedan test koji provjerava da li je taj sa strana zapravo dodiruje taj vp.. 
	// npr moze imati left=v.right ali da je jedno 2 sprata iznad
	// 
	std::set<Viewport*> getAdjecentViewports(Window* window, ViewportBoundary boundary);
	void joinViewport(Window* window, ViewportBoundary boundary);


	// return a ivec4 whose elements correspond to the pixel coordinates of the viewport corners in the order: left, bottom, right, top
	glm::ivec4 getCorners(Window* window);

	void resize(Window* window, ViewportBoundary boundary);




	void resizeWindow(int windowWidth, int windowHeight);
	//	uses normalized coordinates
	void adjustSize(double newLeft, double newBottom, double newRight, double newTop, Window* window);
	void adjustSize(Window* window) { adjustSize(left, bottom, right, top, window); }



};