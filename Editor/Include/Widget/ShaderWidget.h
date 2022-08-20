#pragma once

#include "IMGUIWidgetList.h"

class CShaderWidget :
    public CIMGUIWidgetList
{
public:
    CShaderWidget();
    virtual ~CShaderWidget();

public:
    virtual bool Init() override;

public:
    virtual void SetMaterial(class CMaterial* Mat)
    {
        m_Material = Mat;
    }

    virtual void SetShaderParams(const ShaderParams& Params) = 0;

protected:
    class CMaterial* m_Material;
};

