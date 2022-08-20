#include "BombShader.h"

CBombShader::CBombShader()
{
	SetTypeID<CBombShader>();
}

CBombShader::~CBombShader()
{
}

bool CBombShader::Init()
{
	if (!LoadVertexShader("HDRRenderVS", TEXT("HDRRender.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("RenderBombEffect", TEXT("HDRRender.fx"), SHADER_PATH))
		return false;

	return true;
}
