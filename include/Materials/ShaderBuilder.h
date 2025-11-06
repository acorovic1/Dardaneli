#pragma once

#include <string>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

struct ShaderBuilder
{
    std::stringstream header;   // uniforms, in/out
    std::stringstream body;     // main() body
    std::unordered_set<std::string> declaredUniforms;
    std::unordered_map<int, std::string> nodeVars; // nodeID -> variable name
    int tempCounter = 0;
};
