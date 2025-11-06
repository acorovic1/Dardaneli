#pragma once
#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>

struct ColorMixNode : public ShadingNodes {


    ColorMixNode(int n);

    void draw() override;

    void emitCode(ShaderBuilder& builder, bool visited) override;

    std::vector<int> getInputIds() override;
};
