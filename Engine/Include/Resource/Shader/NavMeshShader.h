#pragma once
#include "GraphicShader.h"
class CNavMeshShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CNavMeshShader();
    virtual ~CNavMeshShader();

public:
    virtual bool Init();
};

