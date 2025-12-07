#pragma once

#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>

struct MetallicOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	MetallicOutputNode(int n);

	void emitCode(ShaderBuilder& builder, bool visited) override;
	std::vector<int>getInputIds() override;
	void draw() override;
	
};


