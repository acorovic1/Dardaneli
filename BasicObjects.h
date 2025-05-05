#pragma once
#pragma once

#include "Mesh.h"

constexpr auto PI = 3.14159265358979323846f;
constexpr auto halfPI = PI/2;

void addPlane();
void addCube();
void addCircle(int numSegments = 32, float radius = 1.0f);
void addSphere(int segments = 32, int rings = 16, float radius = 1.0f); // polulopta
void addCylinder(int numSegments = 8, float height = 2.0f, float radius = 1.0f);
void addCone(int numSegments = 32, float height = 2.0f, float radius = 1.0f);
void addDoughnut(int majorSegments = 32, int minorSegments = 16, float majorRadius = 0.5f, float minorRadius = 0.5f);