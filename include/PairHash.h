#pragma once
#include <unordered_map>   
#include <utility>        
#include <functional>      

namespace std {
	template<>
	struct hash<std::pair<int, int>> {
		std::size_t operator()(const std::pair<int, int>& p) const {
			return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
		}
	};
}
