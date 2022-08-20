#include "ToonShaderWidget.h"
#include "IMGUIComboBox.h"
#include "EngineUtil.h"
#include "Resource/Material/Material.h"

CToonShaderWidget::CToonShaderWidget()	:
	m_RampTexType(nullptr)
{
}

CToonShaderWidget::~CToonShaderWidget()
{
}

bool CToonShaderWidget::Init()
{
	CShaderWidget::Init();

	m_RampTexType = AddWidget<CIMGUIComboBox>("Toon Type", 200.f);

	std::string Str;
	for (size_t i = 0; i < (int)ToonShaderType::Max; ++i)
	{
		Str = CEngineUtil::ToonShaderTypeToString((ToonShaderType)i);
		m_RampTexType->AddItem(Str);
	}

	m_RampTexType->SetSelectCallback(this, &CToonShaderWidget::OnSelectRampTexType);

	return true;
}

void CToonShaderWidget::OnSelectRampTexType(int Idx, const char* Label)
{
	ShaderParams CurParam = m_Material->GetShaderParams();
	CurParam.ToonType = (ToonShaderType)Idx;
	m_Material->SetShaderParams(CurParam);
}

void CToonShaderWidget::SetMaterial(CMaterial* Mat)
{
	CShaderWidget::SetMaterial(Mat);

	ShaderParams CurParams = m_Material->GetShaderParams();

	m_RampTexType->SetSelectIndex((int)CurParams.ToonType);
}

void CToonShaderWidget::SetShaderParams(const ShaderParams& Params)
{
	ToonShaderType Type = Params.ToonType;
	m_RampTexType->SetSelectIndex((int)Type);
}
