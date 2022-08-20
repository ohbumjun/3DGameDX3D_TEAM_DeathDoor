#pragma once

#include "GraphicShader.h"

class CWaterShader :
    public CGraphicShader
{
    friend class CShaderManager;

private:
    CWaterShader();
    virtual ~CWaterShader();

public:
    virtual bool Init() override;
    virtual void SetShaderResources() override;
    virtual void ResetShaderResources() override;
};

