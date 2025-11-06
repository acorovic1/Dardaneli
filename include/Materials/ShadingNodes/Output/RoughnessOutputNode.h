#pragma once

#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>

struct RoughnessOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	RoughnessOutputNode(int n);
	void emitCode(ShaderBuilder& builder, bool visited) override;

	std::vector<int>getInputIds() override;

	void draw() override;

};


