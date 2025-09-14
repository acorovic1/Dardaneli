#pragma once
#include <functional>
#include <memory>
#include "ImGUI/imnodes.h"
#include "ShadingNodesManager.h"
#include "ShaderBuilder.h"


// Pin shapes ImNodesPinShape_TriangleFilled are of type Vec3
// Pin shapes ImNodesPinShape_CircleFilled are of type Float



class ShadingNodes {

protected:
    int id;


public:
	ShadingNodes(int n):id(n)  {
    
    
    }
    virtual ~ShadingNodes() = default;
    virtual void emitCode(ShaderBuilder& builder) = 0;


    virtual void draw() = 0;

    virtual std::vector<int>getInputIds() = 0;


	int getId() { return id; }
};

