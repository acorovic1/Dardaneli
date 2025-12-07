#pragma once

#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>

struct NormalOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	NormalOutputNode(int n);

	void emitCode(ShaderBuilder& builder, bool visited) override;

	std::vector<int>getInputIds() override;

	void draw() override;
	
};


