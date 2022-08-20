#pragma once

#include "ShaderWidget.h"

class CToonShaderWidget :
    public CShaderWidget 
{
public:
    CToonShaderWidget();
    virtual ~CToonShaderWidget();

public:
    virtual bool Init() override;

public:
    void OnSelectRampTexType(int Idx, const char* Label);

public:
    virtual void SetMaterial(class CMaterial* Mat) override;
    virtual void SetShaderParams(const ShaderParams& Params) override;

private:
    class CIMGUIComboBox* m_RampTexType;
};

