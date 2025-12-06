#pragma once

#include "RaytracingBVHNode.h"
#include "Mesh/Mesh.h"

struct RaytracingBVHNode;

class RaytracingBVH {
	std::vector<flatRTNode> nodes;
	std::vector<Triangle> triangles;
	static RaytracingBVH* instancePtr;

	RaytracingBVH() :nodes() {};

	//void DrawTree(RaytracingBVHNode* node, Camera& camera, Shader& shader, int subdivision);


public:

	static RaytracingBVH* getInstance();

	RaytracingBVH(const RaytracingBVH& copy) = delete;
	void operator=(const RaytracingBVH& copy) = delete;

	std::vector<flatRTNode>& getNodes();
	std::vector<Triangle>& getTriangles();
	void Build();
	void findTlasLeaf(BVHNode *objNode);
	// pokusati staviti default vrijednosti kroz objectManager

	//void Refit(Mesh& mesh);
	void Clear();

	//void Draw(Camera& camera, Shader& shader, int subdivision);
	//void DrawLeaves(RaytracingBVHNode* node, Camera& camera, Shader& shader);

	friend flatRTNode BuildMedianSplit(std::vector<flatRTNode>& bvhNodes, int start, int end);

};

extern RaytracingBVH* RaytracingBVHSingleton;