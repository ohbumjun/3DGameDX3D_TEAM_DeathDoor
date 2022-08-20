#include "BlurHorizontalShader.h"

CBlurHorizontalShader::CBlurHorizontalShader()
{
	SetTypeID<CBlurHorizontalShader>();
}

CBlurHorizontalShader::~CBlurHorizontalShader()
{
}

bool CBlurHorizontalShader::Init()
{
	if (!LoadComputeShader("HorizontalFilter", TEXT("Blur.fx"), SHADER_PATH))
	{
		return false;
	}

	return true;
}
