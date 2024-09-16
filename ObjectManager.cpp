#include "ObjectManager.h"

ObjectManager* ObjectManager::instancePtr = nullptr;

 ObjectManager* ObjectManager::getInstance() {
	if (!instancePtr)
		instancePtr = new ObjectManager();
	return instancePtr;
}

 ObjectManager* listOfObjects = ObjectManager::getInstance();

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