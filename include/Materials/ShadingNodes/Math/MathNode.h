#pragma once
#include "ShadingNodes/ShadingNodes.h"


struct MathNode : public ShadingNodes {

	MathNode(int n);

	void emitCode(ShaderBuilder& builder, bool visited) override;

	void draw() override;
	
	std::vector<int>getInputIds() override;

};