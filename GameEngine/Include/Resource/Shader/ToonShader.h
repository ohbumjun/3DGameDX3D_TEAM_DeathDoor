#pragma once

#include "GraphicShader.h"

class CToonShader :
    public CGraphicShader
{
    friend class CShaderManager;

private:
    CToonShader();
    virtual ~CToonShader();

public:
    virtual bool Init() override;
    virtual void SetShaderParams(const ShaderParams& Params) override;

private:
    class CToonCBuffer* m_CBuffer;
};

