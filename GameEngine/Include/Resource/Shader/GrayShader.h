#pragma once

#include "GraphicShader.h"

class CGrayShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CGrayShader();
    virtual ~CGrayShader();

public:
    virtual bool Init();
};

