#pragma once

#include "ShadingNodes.h"
#include <Application.h>

struct AmbientOcclusionOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	AmbientOcclusionOutputNode(int n);
	void draw() override;

	void emitCode(ShaderBuilder& builder, bool visited) override;

	std::vector<int>getInputIds();
	
};


