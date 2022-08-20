#include "GrayShader.h"

CGrayShader::CGrayShader()
{
	SetTypeID<CGrayShader>();
}

CGrayShader::~CGrayShader()
{
}

bool CGrayShader::Init()
{
	if (!LoadVertexShader("GrayVS", TEXT("Gray.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("GrayPS", TEXT("Gray.fx"), SHADER_PATH))
		return false;

	return true;
}
