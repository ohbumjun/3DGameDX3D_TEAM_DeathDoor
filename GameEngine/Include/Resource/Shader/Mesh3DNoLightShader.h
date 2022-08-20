#pragma once
#include "GraphicShader.h"

class CMesh3DNoLightShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CMesh3DNoLightShader();
    virtual ~CMesh3DNoLightShader();

public:
    virtual bool Init();
};

