#include "Object.h"




Object::Object()
{
	index = listOfObjects->getAvailableIndex();
	listOfObjects->addObject(this);
}


GLuint Object::getIndex()const
{
	return index;
};
