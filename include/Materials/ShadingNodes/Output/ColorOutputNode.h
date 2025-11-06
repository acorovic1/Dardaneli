#pragma once

#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>

struct ColorOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	ColorOutputNode(int n);
	void draw() override;

	void emitCode(ShaderBuilder& builder, bool visited) override;
	std::vector<int>getInputIds() override;
};


