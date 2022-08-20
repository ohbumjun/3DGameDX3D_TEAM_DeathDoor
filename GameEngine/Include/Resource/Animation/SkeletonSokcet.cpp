
#include "SkeletonSokcet.h"

CSkeletonSocket::CSkeletonSocket()
{
}

CSkeletonSocket::CSkeletonSocket(const CSkeletonSocket& Socket)
{
	*this = Socket;
}

CSkeletonSocket::~CSkeletonSocket()
{
}

void CSkeletonSocket::Update(const Matrix& matBone)
{
	Matrix	matOffsetRot, matOffset;
	matOffsetRot.Rotation(m_OffsetRot);
	matOffset.Translation(m_Offset);

	m_matSocket = matOffsetRot * matOffset * matBone;
}

CSkeletonSocket* CSkeletonSocket::Clone()
{
	return new CSkeletonSocket(*this);
}

bool CSkeletonSocket::Save(FILE* File)
{
	size_t Length = m_Name.length();
	char Name[256] = {};
	strcpy_s(Name, m_Name.c_str());

	fwrite(&Length, sizeof(size_t), 1, File);
	fwrite(Name, sizeof(char), Length, File);

	char BoneName[256] = {};
	strcpy_s(BoneName, m_BoneName.c_str());
	Length = m_BoneName.length();

	fwrite(&Length, sizeof(size_t), 1, File);
	fwrite(BoneName, sizeof(char), Length, File);

	fwrite(&m_BoneIndex, sizeof(int), 1, File);
	fwrite(&m_Offset, sizeof(Vector3), 1, File);
	fwrite(&m_OffsetRot, sizeof(Vector3), 1, File);
	fwrite(&m_matSocket, sizeof(Matrix), 1, File);


	return true;
}

bool CSkeletonSocket::Load(FILE* File)
{
	size_t Length = -1;
	char Name[256] = {};
	fread(&Length, sizeof(size_t), 1, File);
	fread(&Name, sizeof(char), Length, File);

	m_Name = Name;

	Length = -1;
	fread(&Length, sizeof(size_t), 1, File);
	char BoneName[256] = {};
	fread(&BoneName, sizeof(char), Length, File);

	m_BoneName = BoneName;

	fread(&m_BoneIndex, sizeof(int), 1, File);
	fread(&m_Offset, sizeof(Vector3), 1, File);
	fread(&m_OffsetRot, sizeof(Vector3), 1, File);
	fread(&m_matSocket, sizeof(Matrix), 1, File);

	return true;
}
