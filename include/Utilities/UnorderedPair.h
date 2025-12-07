#pragma once

#include <functional>
#include <utility>

template<typename T>
struct UnorderedPair {

	T first, second;

	UnorderedPair(const T& a, const T& b) {
		if (a < b) {
			first = a;
			second = b;
		}
		else {
			first = b;
			second = a;
		}
	}
	bool operator==(const UnorderedPair<T>& other) const {
		return first == other.first && second == other.second;
	}
	


};


template<typename T>
struct UnorderedPairHash {
	std::size_t operator()(const UnorderedPair<T>& pair) const {
		// Use XOR to make hash order-independent
		std::hash<T> hasher;
		return hasher(pair.first) ^ hasher(pair.second);
	}
};
namespace std {
	template<typename T>
	struct hash<UnorderedPair<T>> {
		std::size_t operator()(const UnorderedPair<T>& pair) const {
			UnorderedPairHash<T> hasher;
			return hasher(pair);
		}
	};
}

