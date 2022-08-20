#pragma once
#include "ComputeShader.h"
class CBloomShader :
    public CComputeShader
{
    friend class CShaderManager;

private:
    CBloomShader();
    virtual ~CBloomShader();

public:
    virtual bool Init() override;
};

