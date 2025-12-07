#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../../libraries/include/glm/glm.hpp"

namespace BVHBuilder
{
	template <typename NodeType>
	inline glm::vec3 ComputeCentroid(NodeType* node)
	{
		return (node->box.min + node->box.max) * 0.5f;
	}

	template <typename NodeType>
	int ChooseSplitAxis(const std::vector<NodeType*>& nodes, int start, int end)
	{
		glm::vec3 minC(std::numeric_limits<float>::max());
		glm::vec3 maxC(std::numeric_limits<float>::lowest());

		for (int i = start; i < end; ++i)
		{
			glm::vec3 c = ComputeCentroid(nodes[i]);
			minC = glm::min(minC, c);
			maxC = glm::max(maxC, c);
		}

		glm::vec3 extent = maxC - minC;
		int axis = 0;
		if (extent.y > extent.x && extent.y >= extent.z) axis = 1;
		else if (extent.z > extent.x && extent.z >= extent.y) axis = 2;
		return axis;
	}

	template <typename NodeType>
	NodeType* BuildMedianSplit(std::vector<NodeType*>& nodes, int start, int end)
	{
		int count = end - start;
		if (count <= 0) return nullptr;
		if (count == 1) return nodes[start];

		int axis = ChooseSplitAxis(nodes, start, end);
		int mid = start + count / 2;
		auto comparator = [axis](NodeType* a, NodeType* b)
		{
			float ca = (a->box.min[axis] + a->box.max[axis]) * 0.5f;
			float cb = (b->box.min[axis] + b->box.max[axis]) * 0.5f;
			return ca < cb;
		};

		std::nth_element(nodes.begin() + start, nodes.begin() + mid, nodes.begin() + end, comparator);

		NodeType* left = BuildMedianSplit(nodes, start, mid);
		NodeType* right = BuildMedianSplit(nodes, mid, end);

		return new NodeType(left, right);
	}

	template <typename PayloadIterator, typename LeafFactory>
	auto BuildBVH(PayloadIterator begin, PayloadIterator end, LeafFactory&& makeLeaf)
	{
		using LeafPtr = decltype(std::declval<LeafFactory>()(*begin));
		using NodeType = std::remove_pointer_t<std::remove_reference_t<LeafPtr>>;

		std::vector<NodeType*> leaves;
		for (auto it = begin; it != end; ++it)
			leaves.push_back(makeLeaf(*it));

		return BuildMedianSplit(leaves, 0, static_cast<int>(leaves.size()));
	}

	template <typename PayloadContainer, typename LeafFactory>
	auto BuildBVH(const PayloadContainer& payloads, LeafFactory&& makeLeaf)
	{
		return BuildBVH(payloads.begin(), payloads.end(), std::forward<LeafFactory>(makeLeaf));
	}
}

