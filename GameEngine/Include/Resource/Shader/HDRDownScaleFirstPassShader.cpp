#include "HDRDownScaleFirstPassShader.h"

CHDRDownScaleFirstPassShader::CHDRDownScaleFirstPassShader()
{
	SetTypeID<CHDRDownScaleFirstPassShader>();
}

CHDRDownScaleFirstPassShader::~CHDRDownScaleFirstPassShader()
{
}

bool CHDRDownScaleFirstPassShader::Init()
{
	if (!LoadComputeShader("DownScaleFirstPass", TEXT("HDRDownScale.fx"), SHADER_PATH))
		return false;

	return true;
}
