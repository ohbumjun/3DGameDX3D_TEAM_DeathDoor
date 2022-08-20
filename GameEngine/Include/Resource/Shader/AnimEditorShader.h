#pragma once
#include "GraphicShader.h"

class CAnimEditorShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CAnimEditorShader();
    virtual ~CAnimEditorShader();

public:
    virtual bool Init();
};

