#pragma once

#include "ComputeShader.h"

class CBlurVerticalShader :
    public CComputeShader
{
    friend class CShaderManager;

private:
    CBlurVerticalShader();
    virtual ~CBlurVerticalShader();

public:
    virtual bool Init() override;
};

