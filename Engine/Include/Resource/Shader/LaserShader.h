#pragma once
#include "GraphicShader.h"

class CLaserShader :
    public CGraphicShader
{
    friend class CShaderManager;

private:
    CLaserShader();
    virtual ~CLaserShader();

public:
    virtual bool Init() override;
};

