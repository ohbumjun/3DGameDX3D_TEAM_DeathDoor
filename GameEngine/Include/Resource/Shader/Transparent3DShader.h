#pragma once

#include "GraphicShader.h"

class CTransparent3DShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CTransparent3DShader();
    virtual ~CTransparent3DShader();

public:
    virtual bool Init();
};

