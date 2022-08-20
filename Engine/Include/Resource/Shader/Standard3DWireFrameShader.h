#pragma once

#include "GraphicShader.h"

class CStandard3DWireFrameShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CStandard3DWireFrameShader();
    virtual ~CStandard3DWireFrameShader();

public:
    virtual bool Init();
};

