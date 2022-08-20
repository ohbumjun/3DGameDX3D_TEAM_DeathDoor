#pragma once

#include "ComputeShader.h"

class CBlurHorizontalShader :
    public CComputeShader
{
    friend class CShaderManager;

private:
    CBlurHorizontalShader();
    virtual ~CBlurHorizontalShader();

public:
    virtual bool Init() override;
};

