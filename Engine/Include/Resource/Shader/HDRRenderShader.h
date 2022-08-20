#pragma once

#include "GraphicShader.h"

class CHDRRenderShader :
    public CGraphicShader
{
    friend class CShaderManager;

private:
    CHDRRenderShader();
    virtual ~CHDRRenderShader();

public:
    virtual bool Init() override;
};

