#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "glm/glm.hpp"

struct TestAABB {
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	TestAABB() = default;
	TestAABB(glm::vec3 minPoint, glm::vec3 maxPoint) : min(minPoint), max(maxPoint) {}

	TestAABB(const TestAABB& a, const TestAABB& b)
	{
		min.x = std::min(a.min.x, b.min.x);
		min.y = std::min(a.min.y, b.min.y);
		min.z = std::min(a.min.z, b.min.z);

		max.x = std::max(a.max.x, b.max.x);
		max.y = std::max(a.max.y, b.max.y);
		max.z = std::max(a.max.z, b.max.z);
	}
};

struct TestNode {
	TestAABB box;
	TestNode* left = nullptr;
	TestNode* right = nullptr;

	explicit TestNode(const TestAABB& bounds) : box(bounds) {}
	TestNode(TestNode* l, TestNode* r)
		: box(l ? l->box : r->box, r ? r->box : l->box), left(l), right(r) {}
};

glm::vec3 ComputeCentroid(const TestAABB& box)
{
	return (box.min + box.max) * 0.5f;
}

int ChooseSplitAxis(const std::vector<TestNode*>& nodes, int start, int end)
{
	glm::vec3 minC(std::numeric_limits<float>::max());
	glm::vec3 maxC(std::numeric_limits<float>::lowest());

	for (int i = start; i < end; ++i)
	{
		glm::vec3 c = ComputeCentroid(nodes[i]->box);
		minC = glm::min(minC, c);
		maxC = glm::max(maxC, c);
	}

	glm::vec3 extent = maxC - minC;
	int axis = 0;
	if (extent.y > extent.x && extent.y >= extent.z) axis = 1;
	else if (extent.z > extent.x && extent.z >= extent.y) axis = 2;
	return axis;
}

TestNode* BuildMedianSplit(std::vector<TestNode*>& nodes, int start, int end)
{
	int count = end - start;
	if (count <= 0) return nullptr;
	if (count == 1) return nodes[start];

	int axis = ChooseSplitAxis(nodes, start, end);
	int mid = start + count / 2;
	auto comparator = [axis](TestNode* a, TestNode* b)
	{
		float ca = (a->box.min[axis] + a->box.max[axis]) * 0.5f;
		float cb = (b->box.min[axis] + b->box.max[axis]) * 0.5f;
		return ca < cb;
	};

	std::nth_element(nodes.begin() + start, nodes.begin() + mid, nodes.begin() + end, comparator);

	TestNode* left = BuildMedianSplit(nodes, start, mid);
	TestNode* right = BuildMedianSplit(nodes, mid, end);

	return new TestNode(left, right);
}

void DestroyTree(TestNode* node)
{
	if (!node) return;
	DestroyTree(node->left);
	DestroyTree(node->right);
	delete node;
}

std::vector<TestAABB> GenerateRandomBounds(size_t count, std::mt19937& rng, float minSize, float maxSize)
{
	std::uniform_real_distribution<float> centerDist(-2000.0f, 2000.0f);
	std::uniform_real_distribution<float> halfSizeDist(minSize, maxSize);

	std::vector<TestAABB> bounds;
	bounds.reserve(count);

	for (size_t i = 0; i < count; ++i)
	{
		glm::vec3 center(centerDist(rng), centerDist(rng), centerDist(rng));
		float halfSize = halfSizeDist(rng);
		glm::vec3 extent(halfSize);
		bounds.emplace_back(center - extent, center + extent);
	}

	return bounds;
}

std::vector<glm::vec3> GenerateRandomVertices(size_t count, std::mt19937& rng, float spread)
{
	std::uniform_real_distribution<float> dist(-spread, spread);
	std::vector<glm::vec3> vertices;
	vertices.reserve(count);

	for (size_t i = 0; i < count; ++i)
		vertices.emplace_back(dist(rng), dist(rng), dist(rng));

	return vertices;
}

TestNode* BuildMedianBVH(const std::vector<TestAABB>& bounds)
{
	if (bounds.empty()) return nullptr;
	std::vector<TestNode*> leaves;
	leaves.reserve(bounds.size());
	for (const auto& b : bounds)
		leaves.push_back(new TestNode(b));
	return BuildMedianSplit(leaves, 0, static_cast<int>(leaves.size()));
}

TestNode* BuildLegacyBVH(const std::vector<TestAABB>& bounds, std::mt19937& rng)
{
	if (bounds.empty()) return nullptr;

	std::vector<TestNode*> nodes;
	nodes.reserve(bounds.size());
	for (const auto& b : bounds)
		nodes.push_back(new TestNode(b));

	int numObjects = static_cast<int>(nodes.size());
	std::uniform_int_distribution<int> axisDist(0, 2);

	while (numObjects > 1) {
		int axis = axisDist(rng);

		if (axis == 0)	std::sort(nodes.begin(), nodes.begin() + numObjects, [](TestNode* a, TestNode* b) {return a->box.min.x < b->box.min.x; });
		else if (axis == 1) std::sort(nodes.begin(), nodes.begin() + numObjects, [](TestNode* a, TestNode* b) {return a->box.min.y < b->box.min.y; });
		else std::sort(nodes.begin(), nodes.begin() + numObjects, [](TestNode* a, TestNode* b) {return a->box.min.z < b->box.min.z; });

		TestNode* newNode = new TestNode(nodes[0], nodes[1]);
		nodes[0] = newNode;
		nodes.erase(nodes.begin() + 1);
		numObjects--;
	}
	return nodes[0];
}

std::vector<TestAABB> BuildVertexBounds(const std::vector<glm::vec3>& vertices, float padding)
{
	std::vector<TestAABB> bounds;
	bounds.reserve(vertices.size());
	glm::vec3 pad(padding);
	for (const auto& v : vertices)
		bounds.emplace_back(v - pad, v + pad);
	return bounds;
}

template <typename BuildFunc>
double MeasureBuild(BuildFunc&& func)
{
	auto start = std::chrono::steady_clock::now();
	TestNode* root = func();
	auto end = std::chrono::steady_clock::now();
	double ms = std::chrono::duration<double, std::milli>(end - start).count();
	DestroyTree(root);
	return ms;
}

int main()
{
	std::mt19937 rng(1337);

	std::vector<size_t> objectCounts = { 16, 128, 512, 2'048, 8'192 };

	std::cout << "=== Object BVH ===\n";
	for (size_t count : objectCounts)
	{
		auto bounds = GenerateRandomBounds(count, rng, 0.25f, 5.0f);
		std::mt19937 legacyRng(static_cast<uint32_t>(count));
		double legacyMs = MeasureBuild([&] { return BuildLegacyBVH(bounds, legacyRng); });
		double improvedMs = MeasureBuild([&] { return BuildMedianBVH(bounds); });
		std::cout << "Objects: " << count << "\n";
		std::cout << "  Legacy   : " << legacyMs << " ms\n";
		std::cout << "  Improved : " << improvedMs << " ms\n";
	}

	std::vector<size_t> vertexCounts = { 65'536, 1'000'000, 2'000'000 };

	std::cout << "\n=== Vertex BVH ===\n";
	for (size_t count : vertexCounts)
	{
		auto vertices = GenerateRandomVertices(count, rng, 3000.0f);
		auto bounds = BuildVertexBounds(vertices, 0.01f);
		std::mt19937 legacyRng(static_cast<uint32_t>(count * 17));
		//double legacyMs = MeasureBuild([&] { return BuildLegacyBVH(bounds, legacyRng); });
		double improvedMs = MeasureBuild([&] { return BuildMedianBVH(bounds); });
		std::cout << "Vertices: " << count << "\n";
		//std::cout << "  Legacy   : " << legacyMs << " ms\n";
		std::cout << "  Improved : " << improvedMs << " ms\n";
	}

	return 0;
}

