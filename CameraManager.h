#pragma once

#include "Camera.h"


class CameraManager {

	std::vector<Camera*> cameras;
	static CameraManager* instancePtr;

	CameraManager() :cameras(0) {};

	
public:

	static CameraManager* getInstance();

	CameraManager(const CameraManager& copy) = delete;
	void operator=(const CameraManager& copy) = delete;

	void addCamera(Camera* Camera);
	void deleteCamera(GLuint index);

	Camera* getCamera(GLuint index);

	std::vector<Camera*>& getAllCameras();

};

extern CameraManager* cameraSingleton;