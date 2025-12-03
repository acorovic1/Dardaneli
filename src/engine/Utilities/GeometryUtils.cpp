#include "GeometryUtils.h"
#include "glm/gtx/vector_angle.hpp"
#include "iostream"
#include <algorithm>

glm::vec3 GeometryUtils::bestSlideDirection(std::vector<glm::vec3> directions, std::vector<glm::vec2> unProjectedDirections, glm::vec2 mainDir)
{
	mainDir = glm::normalize(mainDir);

	float bestDot = -std::numeric_limits<float>::infinity();
	int returnIndex = 0;

	for (int i = 0; i < unProjectedDirections.size(); ++i)
	{
		float dot = glm::dot(mainDir, unProjectedDirections[i]); // ranges from -1 to 1

		if (dot > bestDot)
		{
			bestDot = dot;
			returnIndex = i;
		}
	}

	// If the best direction is opposite, invert it.
	//std::cout << "\n\n\t bestDot " << bestDot;
	if (bestDot < 0.0f)
	{
		//std::cout << "\n\n\tDIRECTION REVERSED";
		return -directions[returnIndex];
	}
	else
		return directions[returnIndex];
}

DVertex* GeometryUtils::findClosestVertex(std::vector<DVertex*> choose, DVertex* a, DVertex* b)
{
	//std::vector<std::pair<DVertex*, float>> sortedA;
	//std::vector<std::pair<DVertex*, float>> sortedB;


	//for (auto vert : choose) {
	//	sortedA.push_back({ vert, glm::length(vert->position - a->position) });
	//	sortedB.push_back({ vert, glm::length(vert->position - b->position) });
	//}


	//std::sort(sortedA.begin(), sortedA.end(),
	//	[](const auto& a, const auto& b) {
	//		return a.second < b.second;
	//	});

	//std::sort(sortedB.begin(), sortedB.end(),
	//	[](const auto& a, const auto& b) {
	//		return a.second < b.second;
	//	});

	//if (sortedA[0].first == sortedB[0].first || sortedA[0].first == sortedB[1].first)
	//	return sortedA[0].first;
	//else if (sortedA[1].first == sortedB[0].first || sortedA[1].first == sortedB[1].first)
	//	return sortedA[1].first;
	//else
	//	return nullptr;


	auto inferEdge = [&](DVertex* mid, DVertex*& out0, DVertex*& out1) -> bool {
		const float eps_rel = 1e-3f;  // relative tolerance for larger quads
		const float eps_abs = 1e-5f;  // absolute floor
		for (size_t i = 0; i < choose.size(); ++i) {
			for (size_t j = i + 1; j < choose.size(); ++j) {
				glm::vec3 u = choose[i]->position;
				glm::vec3 v = choose[j]->position;
				glm::vec3 expected_mid = (u + v) * 0.5f;
				float edgeLen = glm::length(v - u);
				float tol = std::max(eps_abs, edgeLen * eps_rel);
				if (glm::length(expected_mid - mid->position) <= tol) {
					out0 = choose[i];
					out1 = choose[j];
					return true;
				}
			}
		}
		return false;
		};

	DVertex* a0 = nullptr, * a1 = nullptr;
	DVertex* b0 = nullptr, * b1 = nullptr;

	if (!inferEdge(a, a0, a1) || !inferEdge(b, b0, b1))
		return nullptr; // couldn’t confidently infer one of the parent edges

	// If the two inferred edges share a vertex, return that shared original vertex.
	if (a0 == b0 || a0 == b1) return a0;
	if (a1 == b0 || a1 == b1) return a1;
	return nullptr; // opposite edges / no shared corner

}


bool GeometryUtils::splitQuadAlongMidpointsOpposite(
	const std::vector<DVertex*>& quadCorners,
	DVertex* a,
	DVertex* b,
	std::vector<DVertex*>& out1,
	std::vector<DVertex*>& out2)
{
	if (!a || !b)
		return false;

	const float eps_rel = 1e-3f;
	const float eps_abs = 1e-5f;

	// determine which edge (i,j) of quadCorners the midpoint lies on.
	auto inferEdgeIndices = [&](DVertex* mid, int& out_i, int& out_j) -> bool {
		
		const std::pair<int, int> edges[4] = {
			{0,1}, {1,2}, {2,3}, {3,0}
		};
		for (int e = 0; e < 4; ++e) {
			int i = edges[e].first;
			int j = edges[e].second;
			const glm::vec3& u = quadCorners[i]->position;
			const glm::vec3& v = quadCorners[j]->position;
			glm::vec3 expected_mid = (u + v) * 0.5f;
			float edgeLen = glm::length(v - u);
			float tol = std::max(eps_abs, edgeLen * eps_rel);
			if (glm::length(expected_mid - mid->position) <= tol) {
				out_i = i;
				out_j = j;
				return true;
			}
		}
		return false;
		};

	int a_i = -1, a_j = -1;
	int b_i = -1, b_j = -1;
	if (!inferEdgeIndices(a, a_i, a_j) ||
		!inferEdgeIndices(b, b_i, b_j))
		return false; // couldn't infer one of the parent edges

	// If they share a vertex, not opposite edges.
	if (a_i == b_i || a_i == b_j || a_j == b_i || a_j == b_j)
		return false;

	// Identify opposite-edge pairs: (0,1)-(2,3) or (1,2)-(3,0)
	bool isAB_CD = ((a_i == 0 && a_j == 1 && ((b_i == 2 && b_j == 3))) ||
		(b_i == 0 && b_j == 1 && ((a_i == 2 && a_j == 3))));
	bool isBC_DA = ((a_i == 1 && a_j == 2 && ((b_i == 3 && b_j == 0))) ||
		(b_i == 1 && b_j == 2 && ((a_i == 3 && a_j == 0))));

	if (isAB_CD) {
		// a on AB, b on CD (or vice versa)
		// Quad1: [A, a, b, D]
		// Quad2: [a, B, C, b]
		DVertex* A = quadCorners[0];
		DVertex* B = quadCorners[1];
		DVertex* C = quadCorners[2];
		DVertex* D = quadCorners[3];

		// Need to know which of a/b is on AB and which on CD
		bool a_on_AB = ((a_i == 0 && a_j == 1) || (a_i == 1 && a_j == 0));
		DVertex* midAB = a_on_AB ? a : b;
		DVertex* midCD = a_on_AB ? b : a;

		out1 = { A, midAB, midCD, D };
		out2 = { midAB, B, C, midCD };
		return true;
	}
	else if (isBC_DA) {
		// a on BC, b on DA (or vice versa)
		// Quad1: [B, a, b, A]
		// Quad2: [a, C, D, b]
		DVertex* A = quadCorners[0];
		DVertex* B = quadCorners[1];
		DVertex* C = quadCorners[2];
		DVertex* D = quadCorners[3];

		bool a_on_BC = ((a_i == 1 && a_j == 2) || (a_i == 2 && a_j == 1));
		DVertex* midBC = a_on_BC ? a : b;
		DVertex* midDA = a_on_BC ? b : a;

		out1 = { B, midBC, midDA, A };
		out2 = { midBC, C, D, midDA };
		return true;
	}

	// Not an opposite-edge bisecting case we handle.
	return false;
}



