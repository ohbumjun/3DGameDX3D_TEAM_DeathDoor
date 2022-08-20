
#include "NavMeshShader.h"

CNavMeshShader::CNavMeshShader()
{
	SetTypeID<CNavMeshShader>();
}

CNavMeshShader::~CNavMeshShader()
{
}

bool CNavMeshShader::Init()
{
	if (!LoadVertexShader("NavMeshVS", TEXT("NavMesh.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("NavMeshPS", TEXT("NavMesh.fx"), SHADER_PATH))
		return false;

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!CreateInputLayout())
		return false;

	return true;
}
