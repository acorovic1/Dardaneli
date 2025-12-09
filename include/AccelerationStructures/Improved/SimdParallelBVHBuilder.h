#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../../libraries/include/glm/glm.hpp"

#include <omp.h>

namespace SimdParallelBVHBuilder
{
	namespace detail
	{
		template <typename NodeType>
		inline glm::vec3 ComputeCentroid(NodeType* node)
		{
			return (node->box.min + node->box.max) * 0.5f;
		}

		template <typename NodeType>
		struct LeafRecord
		{
			NodeType* node = nullptr;
			glm::vec3 centroid = glm::vec3(0.0f);
		};

		template <typename NodeType>
		inline int ChooseSplitAxis(const std::vector<LeafRecord<NodeType>>& leaves, int start, int end)
		{
			float minX = std::numeric_limits<float>::max();
			float minY = std::numeric_limits<float>::max();
			float minZ = std::numeric_limits<float>::max();

			float maxX = std::numeric_limits<float>::lowest();
			float maxY = std::numeric_limits<float>::lowest();
			float maxZ = std::numeric_limits<float>::lowest();

			for (int i = start; i < end; ++i)
			{
				const glm::vec3 c = leaves[static_cast<size_t>(i)].centroid;

				minX = std::min(minX, c.x);
				minY = std::min(minY, c.y);
				minZ = std::min(minZ, c.z);

				maxX = std::max(maxX, c.x);
				maxY = std::max(maxY, c.y);
				maxZ = std::max(maxZ, c.z);
			}

			const float extentX = maxX - minX;
			const float extentY = maxY - minY;
			const float extentZ = maxZ - minZ;

			int axis = 0;
			if (extentY > extentX && extentY >= extentZ) axis = 1;
			else if (extentZ > extentX && extentZ >= extentY) axis = 2;
			return axis;
		}

		template <typename NodeType>
		NodeType* BuildMedianSplit(std::vector<LeafRecord<NodeType>>& leaves,
			int start,
			int end,
			int depth = 0)
		{
			int count = end - start;
			if (count <= 0) return nullptr;
			if (count == 1) return leaves[static_cast<size_t>(start)].node;

			int axis = ChooseSplitAxis(leaves, start, end);
			int mid = start + count / 2;

			auto comparator = [axis](const LeafRecord<NodeType>& a, const LeafRecord<NodeType>& b)
			{
				return a.centroid[axis] < b.centroid[axis];
			};
			std::nth_element(leaves.begin() + start, leaves.begin() + mid, leaves.begin() + end, comparator);

			NodeType* left = nullptr;
			NodeType* right = nullptr;

			const int threadCount = omp_in_parallel() ? omp_get_num_threads() : omp_get_max_threads();
			const int totalCount = static_cast<int>(leaves.size());
			const int minTaskSize = 4096;
			const int dynamicThreshold = std::max(minTaskSize, totalCount / std::max(1, threadCount * 4));
			bool spawnTasks = count > dynamicThreshold;

			if (spawnTasks)
			{
#pragma omp task shared(left) firstprivate(start, mid)
				{
					left = BuildMedianSplit(leaves, start, mid, depth + 1);
				}
#pragma omp task shared(right) firstprivate(mid, end)
				{
					right = BuildMedianSplit(leaves, mid, end, depth + 1);
				}
#pragma omp taskwait
			}
			else
			{
				left = BuildMedianSplit(leaves, start, mid, depth + 1);
				right = BuildMedianSplit(leaves, mid, end, depth + 1);
			}

			return new NodeType(left, right);
		}
	} // namespace detail

	template <typename PayloadIterator, typename LeafFactory>
	auto BuildBVH(PayloadIterator begin, PayloadIterator end, LeafFactory&& makeLeaf)
	{
		using LeafPtr = decltype(std::declval<LeafFactory>()(*begin));
		using NodeType = std::remove_pointer_t<std::remove_reference_t<LeafPtr>>;
		const ptrdiff_t count = end - begin;
		if (count <= 0) return static_cast<NodeType*>(nullptr);

		std::vector<detail::LeafRecord<NodeType>> leaves(static_cast<size_t>(count));
		NodeType* root = nullptr;

#pragma omp parallel
		{
#pragma omp for schedule(static)
			for (ptrdiff_t i = 0; i < count; ++i)
			{
				NodeType* leaf = makeLeaf(*(begin + i));
				auto& rec = leaves[static_cast<size_t>(i)];
				rec.node = leaf;
				rec.centroid = detail::ComputeCentroid(leaf);
			}

#pragma omp single
			{
				root = detail::BuildMedianSplit(leaves, 0, static_cast<int>(count));
			}
		}

		return root;
	}

	template <typename PayloadContainer, typename LeafFactory>
	auto BuildBVH(const PayloadContainer& payloads, LeafFactory&& makeLeaf)
	{
		return BuildBVH(payloads.begin(), payloads.end(), std::forward<LeafFactory>(makeLeaf));
	}
}