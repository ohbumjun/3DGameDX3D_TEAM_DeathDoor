#include "HDRDownScaleSecondPassShader.h"

CHDRDownScaleSecondPassShader::CHDRDownScaleSecondPassShader()
{
	SetTypeID<CHDRDownScaleSecondPassShader>();
}

CHDRDownScaleSecondPassShader::~CHDRDownScaleSecondPassShader()
{
}

bool CHDRDownScaleSecondPassShader::Init()
{
	if (!LoadComputeShader("DownScaleSecondPass", TEXT("HDRDownScaleSecond.fx"), SHADER_PATH))
		return false;

	return true;
}