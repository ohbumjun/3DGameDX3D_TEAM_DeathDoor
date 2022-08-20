#pragma once

#include "GraphicShader.h"

class CBombShader :
    public CGraphicShader
{
    friend class CShaderManager;

private:
    CBombShader();
    virtual ~CBombShader();

public:
    virtual bool Init() override;
};

