#include "ObjectManager.h"

ObjectManager* ObjectManager::instancePtr = nullptr;

ObjectManager* objectSingleton = ObjectManager::getInstance();

 ObjectManager* ObjectManager::getInstance() {
	if (!instancePtr)
		instancePtr = new ObjectManager();
	return instancePtr;
}

GLuint ObjectManager::getNumberOfObjects()const
{
	return objects.size();
};


GLuint ObjectManager::getAvailableIndex() 
{
	return getNumberOfObjects();

};

void ObjectManager::addObject(Object* object)
{
	objects.push_back(object);
	
}

void ObjectManager::deleteObject(GLuint index) 
{
	objects.erase(objects.begin() + index);
	
};

Object* ObjectManager::getObject(GLuint index)
{
	return objects[index];
}

std::vector<Object*>& ObjectManager::getAllObjects() { return objects; }