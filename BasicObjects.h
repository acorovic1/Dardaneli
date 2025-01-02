#pragma once



#include "Mesh.h"

constexpr auto PI = 3.14159265358979323846f;

void addPlane();
void addCube();
void addCircle(int numSegments = 32, float radius = 1.0f);
void addSphere(int longitude = 32, int latitude = 16, float radius = 1.0f); // polulopta 
void addCylinder(int numSegments = 32, float height = 2.0f,float radius = 0.5f);
void addCone(int numSegments = 32 ,float height = 2.0f ,float radius = 1.0f); 
void addDoughnut(int numSegmentsU = 32, int numSegmentsV = 16, float majorRadius = 0.5f, float minorRadius = 0.25f); 