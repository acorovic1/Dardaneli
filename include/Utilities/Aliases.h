#pragma once

#include <type_traits>
#include <vector>
#include <unordered_set>
#include "DFace.h"
#include "DEdge.h"

template <typename Container>
using EdgeContainer = typename std::enable_if<
    std::is_same<typename Container::value_type, DEdge*>::value
>::type;


template <typename Container>
using FaceContainer = typename std::enable_if<
    std::is_same<typename Container::value_type, DFace*>::value
>::type;


template <typename Container>
using IntContainer = typename std::enable_if<
    std::is_same<typename Container::value_type, int>::value
>::type;
