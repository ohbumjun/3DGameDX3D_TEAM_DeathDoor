#pragma once
#include "Mesh.h"
class CStaticMesh :
    public CMesh
{
	friend class CMeshManager;

protected:
	CStaticMesh();
	virtual ~CStaticMesh();

public:
	virtual bool Init();
	virtual bool LoadMeshFullPathMultibyte(const char* FullPath);
	virtual bool LoadMeshFullPathMultibyte(std::string& OutName, const char* FullPath) override;

public:
	virtual bool ConvertFBX(class CFBXLoader* Loader, const char* FullPath);
	virtual bool SaveMesh(FILE* File);
	virtual bool LoadMesh(FILE* File);
};

