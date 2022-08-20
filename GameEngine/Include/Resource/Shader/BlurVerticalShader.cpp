#include "BlurVerticalShader.h"

CBlurVerticalShader::CBlurVerticalShader()
{
	SetTypeID<CBlurVerticalShader>();
}

CBlurVerticalShader::~CBlurVerticalShader()
{
}

bool CBlurVerticalShader::Init()
{
	if (!LoadComputeShader("VerticalFilter", TEXT("Blur.fx"), SHADER_PATH))
	{
		return false;
	}

	return true;
}
