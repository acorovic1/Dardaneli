#pragma once

#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>


struct ColorNode : public ShadingNodes {
	float color[3] = { 1.0f, 1.0f, 1.0f }; // RGB


	ColorNode(int n);

	void draw() override;

	void emitCode(ShaderBuilder& builder, bool visited) override;

	std::vector<int> getInputIds() override;
};