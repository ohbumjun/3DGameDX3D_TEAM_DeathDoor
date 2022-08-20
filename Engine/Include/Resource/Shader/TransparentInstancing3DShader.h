#pragma once

#include "GraphicShader.h"

class CTransparentInstancing3DShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CTransparentInstancing3DShader();
    virtual ~CTransparentInstancing3DShader();

public:
    virtual bool Init();
};

