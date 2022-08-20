#pragma once

#include "ComputeShader.h"

class CHDRDownScaleFirstPassShader :
    public CComputeShader
{
    friend class CShaderManager;

private:
    CHDRDownScaleFirstPassShader();
    virtual ~CHDRDownScaleFirstPassShader();

public:
    virtual bool Init() override;
};

