#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include "glm/glm.hpp"

#include "AccelerationStructures/Improved/BVHBuilder.h"

struct TestAABB {
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	TestAABB() = default;
	TestAABB(const glm::vec3& point, float pad)
	{
		glm::vec3 extent(pad);
		min = point - extent;
		max = point + extent;
	}

	TestAABB(const glm::vec3& a, const glm::vec3& b)
	{
		min = glm::min(a, b);
		max = glm::max(a, b);
	}

	static TestAABB Union(const TestAABB& a, const TestAABB& b)
	{
		TestAABB result;
		result.min = glm::min(a.min, b.min);
		result.max = glm::max(a.max, b.max);
		return result;
	}

	bool ApproxEquals(const TestAABB& other, float eps = 1e-3f) const
	{
		return glm::all(glm::lessThanEqual(glm::abs(min - other.min), glm::vec3(eps))) &&
			glm::all(glm::lessThanEqual(glm::abs(max - other.max), glm::vec3(eps)));
	}
};

struct TestNode {
	TestAABB box;
	TestNode* left = nullptr;
	TestNode* right = nullptr;

	explicit TestNode(const TestAABB& bounds) : box(bounds) {}
	TestNode(TestNode* l, TestNode* r)
		: box(l && r ? TestAABB::Union(l->box, r->box) : (l ? l->box : r->box))
		, left(l)
		, right(r) {}
};

void DestroyTree(TestNode* node)
{
	if (!node) return;
	DestroyTree(node->left);
	DestroyTree(node->right);
	delete node;
}

size_t CountLeaves(TestNode* node)
{
	if (!node) return 0;
	if (!node->left && !node->right) return 1;
	return CountLeaves(node->left) + CountLeaves(node->right);
}

bool ValidateBounding(TestNode* node, float eps = 1e-3f)
{
	if (!node) return true;
	if (!node->left && !node->right) return true;

	TestAABB expected = node->left && node->right
		? TestAABB::Union(node->left->box, node->right->box)
		: (node->left ? node->left->box : node->right->box);

	if (!node->box.ApproxEquals(expected, eps))
		return false;

	return ValidateBounding(node->left, eps) && ValidateBounding(node->right, eps);
}

template <typename Payloads, typename LeafFactory>
TestNode* BuildIndexedBVH(const Payloads& payloads, LeafFactory&& makeLeaf)
{
	std::vector<int> indices(payloads.size());
	std::iota(indices.begin(), indices.end(), 0);
	return BVHBuilder::BuildBVH(indices.begin(), indices.end(),
		[&payloads, makeLeaf](int idx)
		{
			return makeLeaf(payloads[idx], idx);
		});
}

std::vector<TestAABB> GenerateObjectBounds(size_t count, std::mt19937& rng)
{
	std::uniform_real_distribution<float> centerDist(-200.0f, 200.0f);
	std::uniform_real_distribution<float> halfSizeDist(0.1f, 5.0f);
	std::vector<TestAABB> bounds;
	bounds.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		glm::vec3 center(centerDist(rng), centerDist(rng), centerDist(rng));
		glm::vec3 extent(halfSizeDist(rng));
		bounds.emplace_back(center - extent, center + extent);
	}
	return bounds;
}

std::vector<glm::vec3> GenerateVertices(size_t count, std::mt19937& rng, float spread)
{
	std::uniform_real_distribution<float> dist(-spread, spread);
	std::vector<glm::vec3> vertices;
	vertices.reserve(count);
	for (size_t i = 0; i < count; ++i)
		vertices.emplace_back(dist(rng), dist(rng), dist(rng));
	return vertices;
}

struct EdgePayload {
	glm::vec3 a;
	glm::vec3 b;
};

std::vector<EdgePayload> GenerateEdges(size_t count, std::mt19937& rng, float spread)
{
	std::uniform_real_distribution<float> dist(-spread, spread);
	std::vector<EdgePayload> edges;
	edges.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		edges.push_back({ glm::vec3(dist(rng), dist(rng), dist(rng)),
			glm::vec3(dist(rng), dist(rng), dist(rng)) });
	}
	return edges;
}

struct FacePayload {
	std::array<glm::vec3, 3> verts;
};

std::vector<FacePayload> GenerateFaces(size_t count, std::mt19937& rng, float spread)
{
	std::uniform_real_distribution<float> dist(-spread, spread);
	std::vector<FacePayload> faces;
	faces.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		FacePayload face;
		for (auto& v : face.verts)
			v = glm::vec3(dist(rng), dist(rng), dist(rng));
		faces.push_back(face);
	}
	return faces;
}

std::vector<glm::vec2> GenerateUVs(size_t count, std::mt19937& rng, float spread)
{
	std::uniform_real_distribution<float> dist(-spread, spread);
	std::vector<glm::vec2> uvs;
	uvs.reserve(count);
	for (size_t i = 0; i < count; ++i)
		uvs.emplace_back(dist(rng), dist(rng));
	return uvs;
}

bool RunAndReport(const std::string& label, TestNode* root, size_t expectedLeaves)
{
	bool ok = root && CountLeaves(root) == expectedLeaves && ValidateBounding(root);
	std::cout << label << ": " << (ok ? "PASS" : "FAIL") << " (leaves=" << expectedLeaves << ")\n";
	DestroyTree(root);
	return ok;
}

int main()
{
	std::mt19937 rng(1337);
	bool allOk = true;

	{
		auto bounds = GenerateObjectBounds(512, rng);
		TestNode* root = BuildIndexedBVH(bounds,
			[](const TestAABB& bounds, int) { return new TestNode(bounds); });
		allOk &= RunAndReport("Object BVH", root, bounds.size());
	}

	{
		auto vertices = GenerateVertices(4096, rng, 1000.0f);
		TestNode* root = BuildIndexedBVH(vertices,
			[](const glm::vec3& v, int) { return new TestNode(TestAABB(v, 0.05f)); });
		allOk &= RunAndReport("Vertex BVH", root, vertices.size());
	}

	{
		auto edges = GenerateEdges(2048, rng, 800.0f);
		TestNode* root = BuildIndexedBVH(edges,
			[](const EdgePayload& e, int) { return new TestNode(TestAABB(e.a, e.b)); });
		allOk &= RunAndReport("Edge BVH", root, edges.size());
	}

	{
		auto faces = GenerateFaces(1024, rng, 600.0f);
		TestNode* root = BuildIndexedBVH(faces,
			[](const FacePayload& face, int)
			{
				TestAABB bounds(face.verts[0], face.verts[0]);
				bounds = TestAABB::Union(bounds, TestAABB(face.verts[1], face.verts[1]));
				bounds = TestAABB::Union(bounds, TestAABB(face.verts[2], face.verts[2]));
				return new TestNode(bounds);
			});
		allOk &= RunAndReport("Face BVH", root, faces.size());
	}

	{
		auto uvs = GenerateUVs(4096, rng, 1.0f);
		TestNode* root = BuildIndexedBVH(uvs,
			[](const glm::vec2& uv, int)
			{
				return new TestNode(TestAABB(glm::vec3(uv, 0.0f), 0.005f));
			});
		allOk &= RunAndReport("UV BVH", root, uvs.size());
	}

	std::cout << (allOk ? "All BVH tests passed." : "Some BVH tests failed.") << std::endl;
	return allOk ? 0 : 1;
}



