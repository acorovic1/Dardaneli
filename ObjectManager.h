#pragma once



#include "glad/glad.h"



#include <vector>
class Object;


class ObjectManager {

	std::vector<Object*> objects;

	static ObjectManager* instancePtr ;

	
	ObjectManager():objects(0) {};


public:

	static ObjectManager* getInstance();
	
	

	ObjectManager(const ObjectManager& copy) = delete;
	void operator=(const ObjectManager& copy) = delete;



	GLuint getNumberOfObjects()const;
	GLuint getAvailableIndex();

	void addObject(Object* object);
	void deleteObject(GLuint index);

	Object* getObject(GLuint index);

	std::vector<Object*>& getAllObjects();

};


extern ObjectManager* objectSingleton;