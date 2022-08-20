#pragma once

#include "GraphicShader.h"

class CToonInstancingShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CToonInstancingShader();
    virtual ~CToonInstancingShader();

public:
    virtual bool Init();
    virtual void SetShaderParams(const ShaderParams& Params) override;

private:
    class CToonCBuffer* m_CBuffer;
};

