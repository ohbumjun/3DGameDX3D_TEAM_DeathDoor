#pragma once

#include "GraphicShader.h"

class COutlineShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    COutlineShader();
    virtual ~COutlineShader();

public:
    virtual bool Init();
};

