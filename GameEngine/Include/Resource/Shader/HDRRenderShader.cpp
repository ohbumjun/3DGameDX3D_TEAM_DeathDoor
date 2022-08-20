#include "HDRRenderShader.h"

CHDRRenderShader::CHDRRenderShader()
{
	SetTypeID<CHDRRenderShader>();
}

CHDRRenderShader::~CHDRRenderShader()
{
}

bool CHDRRenderShader::Init()
{
	if (!LoadVertexShader("HDRRenderVS", TEXT("HDRRender.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("HDRRenderPS", TEXT("HDRRender.fx"), SHADER_PATH))
		return false;

	return true;
}
