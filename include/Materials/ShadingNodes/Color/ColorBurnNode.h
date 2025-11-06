#pragma once


#pragma once
#include "ShadingNodes/ShadingNodes.h"
#include <Application.h>

struct ColorBurnNode : public ShadingNodes {


    ColorBurnNode(int n);

    void draw() override;
    
    void emitCode(ShaderBuilder& builder, bool visited) override;

    std::vector<int> getInputIds();

       
};
