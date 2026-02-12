#pragma once
#pragma once

#include "Mesh/Mesh.h"

constexpr auto PI = 3.14159265358979323846f;
constexpr auto halfPI = PI/2;


/// <summary>
//  add UVVertex* to loops in the following objects: Circle, Sphere, Cylinder, Cone, Doughnut
/// </summary>

void addPlane();
void addCube();
void addCircle(int numSegments = 16, float radius = 1.0f);

// last vertex's disk is wrongly implemented or smth
void addSphere(int segments = 10, int rings =10, float radius = 1.0f); 
void addCylinder(int numSegments = 16, float height = 2.0f, float radius = 1.0f);

// face selection ne radi
void addCone(int numSegments = 8, float height = 2.0f, float radius = 1.0f);

// object converges into 1 point in the middle (no hole)
void addDoughnut(int majorSegments = 16, int minorSegments = 16, float majorRadius = 0.5f, float minorRadius = 0.5f);