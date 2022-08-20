
#include "AnimEditorShader.h"

CAnimEditorShader::CAnimEditorShader()
{
	SetTypeID<CAnimEditorShader>();
}

CAnimEditorShader::~CAnimEditorShader()
{
}

bool CAnimEditorShader::Init()
{
	if (!LoadVertexShader("LightAccVS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("AnimEditorRenderPS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	return true;
}
