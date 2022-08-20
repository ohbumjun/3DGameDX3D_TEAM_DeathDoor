#pragma once

#include "ComputeShader.h"

class CHDRDownScaleSecondPassShader :
    public CComputeShader
{
    friend class CShaderManager;

private:
    CHDRDownScaleSecondPassShader();
    virtual ~CHDRDownScaleSecondPassShader();

public:
    virtual bool Init() override;
};

