#include "OutlineShader.h"

COutlineShader::COutlineShader()
{
	SetTypeID<COutlineShader>();
}

COutlineShader::~COutlineShader()
{
}

bool COutlineShader::Init()
{
	if (!LoadVertexShader("OutlineVS", TEXT("Outline.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("OutlinePS", TEXT("Outline.fx"), SHADER_PATH))
		return false;

	return true;
}
