#pragma once

#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>


struct ValueNode : public ShadingNodes {

	float value;

	ValueNode(int n);

	void emitCode(ShaderBuilder& builder, bool visited) override;

	void draw() override;

	std::vector<int>getInputIds() override;


};