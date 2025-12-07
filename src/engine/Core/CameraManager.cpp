#include "CameraManager.h"

CameraManager* CameraManager::instancePtr = nullptr;

CameraManager* cameraSingleton = CameraManager::getInstance();

CameraManager* CameraManager::getInstance() {
	if (!instancePtr)
		instancePtr = new CameraManager();
	return instancePtr;
}

void CameraManager::addCamera(Camera* object)
{
	cameras.push_back(object);
}

void CameraManager::deleteCamera(GLuint index)
{
	cameras.erase(cameras.begin() + index);
};

Camera* CameraManager::getCamera(GLuint index)
{
	return cameras[index];
}

Camera* CameraManager::getCamera(std::string name)
{
	for (auto& camera : cameras)
	{
		if (camera->getName() == name)
			return camera;
	}
	std::cout << "Camera with name " << name << " not found.\n";
	return nullptr;
}

std::vector<Camera*>& CameraManager::getAllCameras() { return cameras; }