#pragma once

#include "ComputeShader.h"

class CHDRDownScaleSecondPathShader :
    public CComputeShader
{
    friend class CShaderManager;

private:
    CHDRDownScaleSecondPathShader();
    virtual ~CHDRDownScaleSecondPathShader();

public:
    virtual bool Init() override;
};

