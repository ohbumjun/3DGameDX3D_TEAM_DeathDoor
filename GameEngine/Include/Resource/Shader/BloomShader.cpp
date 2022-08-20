#include "BloomShader.h"

CBloomShader::CBloomShader()
{
	SetTypeID<CBloomShader>();
}

CBloomShader::~CBloomShader()
{
}

bool CBloomShader::Init()
{
	if (!LoadComputeShader("BrightPass", TEXT("Bloom.fx"), SHADER_PATH))
	{
		return false;
	}

	return true;
}
