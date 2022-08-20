
#include "AnimationSequence.h"
#include "FBXLoader.h"
#include "../../PathManager.h"
#include "../Shader/StructuredBuffer.h"

CAnimationSequence::CAnimationSequence() :
	m_Scene(nullptr),
	m_End(false),
	m_KeyFrameBuffer(nullptr),
	m_PlayScale(1.f),
	m_PlayTime(1.f),
	m_Loop(false),
	m_StartTime(0.f),
	m_EndTime(0.f),
	m_TimeLength(0.f),
	m_FrameTime(0.f),
	m_StartFrame(0),
	m_EndFrame(0),
	m_FrameLength(0),
	m_FrameMode(0),
	m_ChangeFrame(0)
{
}

CAnimationSequence::CAnimationSequence(const CAnimationSequence& anim)
{
	*this = anim;

	m_KeyFrameBuffer = nullptr;

	for (size_t i = 0; i < m_vecKeyFrame.size(); ++i)
	{
		++m_vecKeyFrame[i]->iRefCount;
	}
}

CAnimationSequence::~CAnimationSequence()
{
	SAFE_DELETE(m_KeyFrameBuffer);

	for (size_t i = 0; i < m_vecKeyFrame.size(); ++i)
	{
		--m_vecKeyFrame[i]->iRefCount;

		if (m_vecKeyFrame[i]->iRefCount == 0)
			delete	m_vecKeyFrame[i];
	}

	m_vecKeyFrame.clear();
}

CAnimationSequence* CAnimationSequence::Clone()
{
	return new CAnimationSequence(*this);
}

bool CAnimationSequence::IsSequenceEnd() const
{
	return m_End;
}

void CAnimationSequence::GetKeyFrame(std::vector<BoneKeyFrame*>& vecFrame)
{
	vecFrame = m_vecKeyFrame;
}

bool CAnimationSequence::LoadFbxAnimation(const char* pFullPath, bool bLoop)
{
	CFBXLoader	loader;

	loader.LoadFBX(pFullPath, false);

	// �ִϸ��̼� Ŭ���� �߰��Ѵ�.
	const std::vector<PFBXANIMATIONCLIP>* pvecClip = loader.GetClips();

	memset(m_FullPath, 0, MAX_PATH);
	strcpy_s(m_FullPath, pFullPath);

	return CreateSequence(bLoop, pvecClip->front());
}

bool CAnimationSequence::Save(const TCHAR* pFileName,
	const std::string& strPathName)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveMultibyte(strFileName, strPathName);
}

bool CAnimationSequence::SaveMultibyte(const char* pFileName,
	const std::string& strPathName)
{
	const PathInfo* pPath = CPathManager::GetInst()->FindPath(strPathName);

	char	strFullPath[MAX_PATH] = {};

	if (pPath)
		strcpy_s(strFullPath, pPath->PathMultibyte);

	strcat_s(strFullPath, pFileName);

	return SaveFullPathMultibyte(strFullPath);
}

bool CAnimationSequence::SaveFullPath(const TCHAR* pFullPath)
{
	char	strFullPath[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, pFullPath, -1, strFullPath, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveFullPathMultibyte(strFullPath);
}


bool CAnimationSequence::Save(FILE* pFile)
{
	CRef::Save(pFile);

	// Full Path ���� ����
	size_t PathLength = strlen(m_FullPath);
	fwrite(&PathLength, sizeof(size_t), 1, pFile);
	fwrite(m_FullPath, sizeof(char), PathLength, pFile);

	// FileName ���� ����
	size_t FileNameLength = strlen(m_FileName);
	fwrite(&FileNameLength, sizeof(size_t), 1, pFile);
	fwrite(m_FileName, sizeof(char), FileNameLength, pFile);

	size_t	iLength = m_Name.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_Name.c_str(), sizeof(char), iLength, pFile);

	fwrite(&m_Loop, sizeof(bool), 1, pFile);
	fwrite(&m_StartTime, sizeof(float), 1, pFile);
	fwrite(&m_EndTime, sizeof(float), 1, pFile);
	fwrite(&m_TimeLength, sizeof(float), 1, pFile);
	fwrite(&m_FrameTime, sizeof(float), 1, pFile);
	fwrite(&m_PlayTime, sizeof(float), 1, pFile);
	fwrite(&m_PlayScale, sizeof(float), 1, pFile);
	fwrite(&m_StartFrame, sizeof(int), 1, pFile);
	fwrite(&m_EndFrame, sizeof(int), 1, pFile);
	fwrite(&m_FrameLength, sizeof(int), 1, pFile);
	fwrite(&m_FrameMode, sizeof(int), 1, pFile);
	fwrite(&m_ChangeFrame, sizeof(int), 1, pFile);

	// Bone ���� 
	size_t	iCount = m_vecKeyFrame.size();

	fwrite(&iCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iCount; ++i)
	{
		fwrite(&m_vecKeyFrame[i]->iBoneIndex, sizeof(int), 1,
			pFile);

		// �� Bone �� ���ϰ� �ִ� Frame ���� 
		size_t	iFrameCount = m_vecKeyFrame[i]->vecKeyFrame.size();

		fwrite(&iFrameCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iFrameCount; ++j)
		{
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->dTime, sizeof(double), 1, pFile);
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->vPos, sizeof(Vector3), 1, pFile);
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->vScale, sizeof(Vector3), 1, pFile);
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->vRot, sizeof(Vector4), 1, pFile);
		}
	}

	return true;
}

bool CAnimationSequence::Load(FILE* pFile)
{
	CRef::Load(pFile);

	// Full Path ���� ����
	size_t PathLength;
	fread(&PathLength, sizeof(size_t), 1, pFile);
	fread(m_FullPath, sizeof(char), PathLength, pFile);

	// FileName ���� ����
	size_t FileNameLength;
	fread(&FileNameLength, sizeof(size_t), 1, pFile);
	fread(m_FileName, sizeof(char), FileNameLength, pFile);

	size_t	iLength = 0;
	fread(&iLength, sizeof(size_t), 1, pFile);
	char	strName[256] = {};
	fread(strName, sizeof(char), iLength, pFile);

	SetName(strName);

	bool	bLoop = true;
	fread(&bLoop, sizeof(bool), 1, pFile);
	m_Loop = bLoop;
	fread(&m_StartTime, sizeof(float), 1, pFile);
	fread(&m_EndTime, sizeof(float), 1, pFile);
	fread(&m_TimeLength, sizeof(float), 1, pFile);
	fread(&m_FrameTime, sizeof(float), 1, pFile);
	fread(&m_PlayTime, sizeof(float), 1, pFile);
	fread(&m_PlayScale, sizeof(float), 1, pFile);
	fread(&m_StartFrame, sizeof(int), 1, pFile);
	fread(&m_EndFrame, sizeof(int), 1, pFile);
	fread(&m_FrameLength, sizeof(int), 1, pFile);
	fread(&m_FrameMode, sizeof(int), 1, pFile);
	fread(&m_ChangeFrame, sizeof(int), 1, pFile);

	size_t	iCount = 0;

	fread(&iCount, sizeof(size_t), 1, pFile);

	// std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(iCount * m_FrameLength);

	// ������ m_vecKeyFrame �� ��� �����ش�.
	size_t VecKeyFrameSize = m_vecKeyFrame.size();

	for (size_t i = 0; i < VecKeyFrameSize; ++i)
	{
		SAFE_DELETE(m_vecKeyFrame[i]);
	}

	// Bone ���� 
	for (size_t i = 0; i < iCount; ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;
		m_vecKeyFrame.push_back(pBoneKeyFrame);

		fread(&pBoneKeyFrame->iBoneIndex, sizeof(int), 1,
			pFile);

		size_t	iBoneFrameCount = 0;

		fread(&iBoneFrameCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iBoneFrameCount; ++j)
		{
			KeyFrame* pKeyFrame = new KeyFrame;
			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

			fread(&pKeyFrame->dTime, sizeof(double), 1, pFile);
			fread(&pKeyFrame->vPos, sizeof(Vector3), 1, pFile);
			fread(&pKeyFrame->vScale, sizeof(Vector3), 1, pFile);
			fread(&pKeyFrame->vRot, sizeof(Vector4), 1, pFile);

			if (j < m_FrameLength)
			{
				AnimationFrameTrans	tFrame = {};
				tFrame.vScale = Vector4(pKeyFrame->vScale.x, pKeyFrame->vScale.y,
					pKeyFrame->vScale.z, 1.f);
				tFrame.vTranslate = Vector4(pKeyFrame->vPos.x, pKeyFrame->vPos.y,
					pKeyFrame->vPos.z, 1.f);
				tFrame.qRot = pKeyFrame->vRot;

				m_vecFrameTrans[i * m_FrameLength + j] = tFrame;
			}
		}
	}

	SAFE_DELETE(m_KeyFrameBuffer);

	m_KeyFrameBuffer = new CStructuredBuffer;

	m_KeyFrameBuffer->Init("KeyFrameBuffer", (unsigned int)sizeof(AnimationFrameTrans),
		(unsigned int)m_vecFrameTrans.size(), 13, true, (int)Buffer_Shader_Type::Compute);

	m_KeyFrameBuffer->UpdateBuffer(&m_vecFrameTrans[0],
		(unsigned int)m_vecFrameTrans.size());

	return true;
}

bool CAnimationSequence::SaveFullPathMultibyte(
	const char* pFullPath)
{
	FILE* pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	size_t	iLength = m_Name.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_Name.c_str(), sizeof(char), iLength, pFile);

	fwrite(&m_Loop, sizeof(bool), 1, pFile);
	fwrite(&m_StartTime, sizeof(float), 1, pFile);
	fwrite(&m_EndTime, sizeof(float), 1, pFile);
	fwrite(&m_TimeLength, sizeof(float), 1, pFile);
	fwrite(&m_FrameTime, sizeof(float), 1, pFile);
	fwrite(&m_PlayTime, sizeof(float), 1, pFile);
	fwrite(&m_PlayScale, sizeof(float), 1, pFile);
	fwrite(&m_StartFrame, sizeof(int), 1, pFile);
	fwrite(&m_EndFrame, sizeof(int), 1, pFile);
	fwrite(&m_FrameLength, sizeof(int), 1, pFile);
	fwrite(&m_FrameMode, sizeof(int), 1, pFile);
	fwrite(&m_ChangeFrame, sizeof(int), 1, pFile);

	size_t	iCount = m_vecKeyFrame.size();

	fwrite(&iCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iCount; ++i)
	{
		fwrite(&m_vecKeyFrame[i]->iBoneIndex, sizeof(int), 1,
			pFile);

		size_t	iFrameCount = m_vecKeyFrame[i]->vecKeyFrame.size();

		fwrite(&iFrameCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iFrameCount; ++j)
		{
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->dTime, sizeof(double), 1, pFile);
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->vPos, sizeof(Vector3), 1, pFile);
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->vScale, sizeof(Vector3), 1, pFile);
			fwrite(&m_vecKeyFrame[i]->vecKeyFrame[j]->vRot, sizeof(Vector4), 1, pFile);
		}
	}

	fclose(pFile);

	return true;
}

bool CAnimationSequence::Load(const TCHAR* pFileName,
	const std::string& strPathName)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadMultibyte(strFileName, strPathName);
}

bool CAnimationSequence::LoadMultibyte(const char* pFileName,
	const std::string& strPathName)
{
	const PathInfo* pPath = CPathManager::GetInst()->FindPath(strPathName);

	char	strFullPath[MAX_PATH] = {};

	if (pPath)
		strcpy_s(strFullPath, pPath->PathMultibyte);

	strcat_s(strFullPath, pFileName);

	return LoadFullPathMultibyte(strFullPath);
}

bool CAnimationSequence::LoadFullPath(const TCHAR* pFullPath)
{
	char	strFullPath[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, pFullPath, -1, strFullPath, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadFullPathMultibyte(strFullPath);
}

bool CAnimationSequence::LoadFullPathMultibyte(const char* pFullPath)
{
	FILE* pFile = NULL;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	strcpy_s(m_FullPath, pFullPath);

	size_t	iLength = 0;
	fread(&iLength, sizeof(size_t), 1, pFile);
	char	strName[256] = {};
	fread(strName, sizeof(char), iLength, pFile);

	SetName(strName);

	bool	bLoop = true;
	fread(&bLoop, sizeof(bool), 1, pFile);
	m_Loop = bLoop;
	fread(&m_StartTime, sizeof(float), 1, pFile);
	fread(&m_EndTime, sizeof(float), 1, pFile);
	fread(&m_TimeLength, sizeof(float), 1, pFile);
	fread(&m_FrameTime, sizeof(float), 1, pFile);
	fread(&m_PlayTime, sizeof(float), 1, pFile);
	fread(&m_PlayScale, sizeof(float), 1, pFile);
	fread(&m_StartFrame, sizeof(int), 1, pFile);
	fread(&m_EndFrame, sizeof(int), 1, pFile);
	fread(&m_FrameLength, sizeof(int), 1, pFile);
	fread(&m_FrameMode, sizeof(int), 1, pFile);
	fread(&m_ChangeFrame, sizeof(int), 1, pFile);
	
	// (OBJ)
	// �ڽ��� Frame Mode �� ���缭, Play Time �� �����Ѵ�.
	// ex) �ڽ��� 24 FrameMode => �� 96���� Frame => 96 / 24 => 4���� ����ð�
	// m_PlayTime = (float)(m_EndFrame - m_StartFrame + 1) / (float)m_FrameMode;

	size_t	iCount = 0;

	fread(&iCount, sizeof(size_t), 1, pFile);

	std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(iCount * m_FrameLength);

	for (size_t i = 0; i < iCount; ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;
		m_vecKeyFrame.push_back(pBoneKeyFrame);

		fread(&pBoneKeyFrame->iBoneIndex, sizeof(int), 1,
			pFile);

		size_t	iBoneFrameCount = 0;

		fread(&iBoneFrameCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iBoneFrameCount; ++j)
		{
			KeyFrame* pKeyFrame = new KeyFrame;
			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

			fread(&pKeyFrame->dTime, sizeof(double), 1, pFile);
			fread(&pKeyFrame->vPos, sizeof(Vector3), 1, pFile);
			fread(&pKeyFrame->vScale, sizeof(Vector3), 1, pFile);
			fread(&pKeyFrame->vRot, sizeof(Vector4), 1, pFile);

			if (j < m_FrameLength)
			{
				AnimationFrameTrans	tFrame = {};
				tFrame.vScale = Vector4(pKeyFrame->vScale.x, pKeyFrame->vScale.y,
					pKeyFrame->vScale.z, 1.f);
				tFrame.vTranslate = Vector4(pKeyFrame->vPos.x, pKeyFrame->vPos.y,
					pKeyFrame->vPos.z, 1.f);
				tFrame.qRot = pKeyFrame->vRot;

				m_vecFrameTrans[i * m_FrameLength + j] = tFrame;
			}
		}
	}

	m_KeyFrameBuffer = new CStructuredBuffer;

	m_KeyFrameBuffer->Init("KeyFrameBuffer", sizeof(AnimationFrameTrans),
		(unsigned int)m_vecFrameTrans.size(), 13, true, (int)Buffer_Shader_Type::Compute);

	m_KeyFrameBuffer->UpdateBuffer(&m_vecFrameTrans[0],
		(unsigned int)m_vecFrameTrans.size());

	fclose(pFile);

	return true;
}

char* CAnimationSequence::GetSequenceFileNameMultibyte()
{
	char	Ext[_MAX_EXT] = {};
	char FileName[MAX_PATH] = {};
	char FileNameFullName[MAX_PATH] = {};

	_splitpath_s(m_FullPath, nullptr, 0, nullptr, 0, FileName, MAX_PATH, Ext, _MAX_EXT);

	strcpy_s(FileNameFullName, FileName);
	// strcat_s(FileNameFullName, Ext);

	return FileNameFullName;
}

void CAnimationSequence::DummyLoad(FILE* pFile)
{
	CRef::Load(pFile);

	// Full Path ���� ����
	size_t PathLength;
	fread(&PathLength, sizeof(size_t), 1, pFile);
	fread(m_FullPath, sizeof(char), PathLength, pFile);

	// FileName ���� ����
	size_t FileNameLength;
	fread(&FileNameLength, sizeof(size_t), 1, pFile);
	fread(m_FileName, sizeof(char), FileNameLength, pFile);

	size_t	iLength = 0;
	fread(&iLength, sizeof(size_t), 1, pFile);
	char	strName[256] = {};
	fread(strName, sizeof(char), iLength, pFile);

	SetName(strName);

	bool	bLoop = true;
	fread(&bLoop, sizeof(bool), 1, pFile);
	m_Loop = bLoop;
	fread(&m_StartTime, sizeof(float), 1, pFile);
	fread(&m_EndTime, sizeof(float), 1, pFile);
	fread(&m_TimeLength, sizeof(float), 1, pFile);
	fread(&m_FrameTime, sizeof(float), 1, pFile);
	fread(&m_PlayTime, sizeof(float), 1, pFile);
	fread(&m_PlayScale, sizeof(float), 1, pFile);
	fread(&m_StartFrame, sizeof(int), 1, pFile);
	fread(&m_EndFrame, sizeof(int), 1, pFile);
	fread(&m_FrameLength, sizeof(int), 1, pFile);
	fread(&m_FrameMode, sizeof(int), 1, pFile);
	fread(&m_ChangeFrame, sizeof(int), 1, pFile);

	size_t	iCount = 0;

	fread(&iCount, sizeof(size_t), 1, pFile);

	// std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(iCount * m_FrameLength);

	// ������ m_vecKeyFrame �� ��� �����ش�.
	size_t VecKeyFrameSize = m_vecKeyFrame.size();

	for (size_t i = 0; i < VecKeyFrameSize; ++i)
	{
		SAFE_DELETE(m_vecKeyFrame[i]);
	}

	for (size_t i = 0; i < iCount; ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;

		fread(&pBoneKeyFrame->iBoneIndex, sizeof(int), 1,
			pFile);

		size_t	iBoneFrameCount = 0;

		fread(&iBoneFrameCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iBoneFrameCount; ++j)
		{
			KeyFrame* pKeyFrame = new KeyFrame;
			fread(&pKeyFrame->dTime, sizeof(double), 1, pFile);
			fread(&pKeyFrame->vPos, sizeof(Vector3), 1, pFile);
			fread(&pKeyFrame->vScale, sizeof(Vector3), 1, pFile);
			fread(&pKeyFrame->vRot, sizeof(Vector4), 1, pFile);
			SAFE_DELETE(pKeyFrame);
		}
		SAFE_DELETE(pBoneKeyFrame);
	}

	m_vecKeyFrame.clear();
}

void CAnimationSequence::SetPlayScale(float fScale)
{
	m_PlayScale = fScale;
}

void CAnimationSequence::SetPlayTime(float fTime)
{
	m_PlayTime = fTime;

	m_EndTime = m_PlayTime;
	m_TimeLength = m_PlayTime;

	m_FrameTime = m_PlayTime / m_FrameLength;

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t i = 0; i < m_vecKeyFrame.size(); ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = m_vecKeyFrame[i];

		for (size_t j = 0; j < pBoneKeyFrame->vecKeyFrame.size(); ++j)
		{
			KeyFrame* pKeyFrame = pBoneKeyFrame->vecKeyFrame[j];

			pKeyFrame->dTime = j * m_FrameTime;
		}
	}
}

void CAnimationSequence::SetAnimationFullPathMultibyte(const char* pFullPath)
{
	strcpy_s(m_FullPath, pFullPath);
}

void CAnimationSequence::SetAnimationFileNameMultibyte(const char* pFileName)
{
	strcpy_s(m_FileName, pFileName);
}

bool CAnimationSequence::CreateSequence(bool bLoop,
	_tagFbxAnimationClip* pClip)
{
	// ���ڷ� ���� �ִϸ��̼� �ɼ������� �����Ѵ�.
	m_Loop = bLoop;
	SetName(pClip->strName);

	m_ChangeFrame = 0;

	switch (pClip->eTimeMode)
	{
	case FbxTime::eFrames24:
		m_FrameMode = 24;
		break;
	case FbxTime::eFrames30:
		m_FrameMode = 30;
		break;
	case FbxTime::eFrames50:
		m_FrameMode = 50;
		break;
	case FbxTime::eFrames60:
		m_FrameMode = 60;
		break;
	}

	// FBXANIMATIONCLIP�� �ִ� starttime �� endtime �� �̿��Ͽ� keyframe �� ���´�.
	m_StartFrame = (int)pClip->tStart.GetFrameCount(pClip->eTimeMode);
	m_EndFrame = (int)pClip->tEnd.GetFrameCount(pClip->eTimeMode);
	m_FrameLength = m_EndFrame - m_StartFrame + 1;

	// �ð� ������ �������ش�.
	m_StartTime = 0.f;

	// (OBJ) �ڵ� ���� �߰�
	// �ش� Sequence �� Frame Mode, Frame ������ ����Ͽ� Play Time �ٽ� ����
	m_PlayTime = (float)(m_EndFrame - m_StartFrame + 1) / (float)m_FrameMode;
	
	m_EndTime = m_PlayTime;
	m_TimeLength = m_PlayTime;

	m_FrameTime = m_PlayTime / m_FrameLength;

	std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(pClip->vecBoneKeyFrame.size() * m_FrameLength);

	// �� 3�� Ű������ 5���� ���
	// ��0Ű0|��0Ű1|��0Ű2|��0Ű3|��0Ű4|��1Ű0|��1Ű1|��1Ű2|��1Ű3|��1Ű4|��2Ű0|��2Ű1|��2Ű2|��2Ű3|��2Ű4

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t i = 0; i < pClip->vecBoneKeyFrame.size(); ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;

		pBoneKeyFrame->iBoneIndex = pClip->vecBoneKeyFrame[i].iBoneIndex;

		m_vecKeyFrame.push_back(pBoneKeyFrame);

		// �Ʒ����� Ű������ ������ �����Ѵ�.
		pBoneKeyFrame->vecKeyFrame.reserve(pClip->vecBoneKeyFrame[i].vecKeyFrame.size());

		for (size_t j = 0; j < pClip->vecBoneKeyFrame[i].vecKeyFrame.size(); ++j)
		{
			KeyFrame* pKeyFrame = new KeyFrame;

			pKeyFrame->dTime = j * m_FrameTime;

			// ���� ���� Ű �����ӿ� �ش��ϴ� ��� ������ ���´�.
			FbxAMatrix		mat = pClip->vecBoneKeyFrame[i].vecKeyFrame[j].matTransform;

			FbxVector4		vPos, vScale;
			FbxQuaternion	qRot;

			// ��ķκ��� ��ġ, ũ��, ȸ�� ������ ���´�.
			vPos = mat.GetT();
			vScale = mat.GetS();
			qRot = mat.GetQ();

			pKeyFrame->vScale = Vector3((float)vScale.mData[0], (float)vScale.mData[1],
				(float)vScale.mData[2]);
			pKeyFrame->vPos = Vector3((float)vPos.mData[0], (float)vPos.mData[1],
				(float)vPos.mData[2]);
			pKeyFrame->vRot = Vector4((float)qRot.mData[0], (float)qRot.mData[1],
				(float)qRot.mData[2], (float)qRot.mData[3]);

			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

			if (j < m_FrameLength)
			{
				AnimationFrameTrans	tFrame = {};
				tFrame.vScale = Vector4(pKeyFrame->vScale.x, pKeyFrame->vScale.y,
					pKeyFrame->vScale.z, 1.f);
				tFrame.vTranslate = Vector4(pKeyFrame->vPos.x, pKeyFrame->vPos.y,
					pKeyFrame->vPos.z, 1.f);
				tFrame.qRot = pKeyFrame->vRot;

				m_vecFrameTrans[i * m_FrameLength + j] = tFrame;
			}

			else
				int a = 10;
		}
	}

	m_KeyFrameBuffer = new CStructuredBuffer;

	m_KeyFrameBuffer->Init("KeyFrameBuffer", (unsigned int)sizeof(AnimationFrameTrans),
		(unsigned int)m_vecFrameTrans.size(), 13, true, (int)Buffer_Shader_Type::Compute);

	m_KeyFrameBuffer->UpdateBuffer(&m_vecFrameTrans[0],
		(int)m_vecFrameTrans.size());

	if (strlen(m_FullPath) != 0)
	{
		char	strAnimName[MAX_PATH] = {};

		_splitpath_s(m_FullPath, 0, 0, 0, 0, strAnimName, MAX_PATH, 0, 0);

		char	strAnimPath[MAX_PATH] = {};
		strcpy_s(strAnimPath, m_FullPath);

		int	iLength = (int)strlen(strAnimPath);
		for (int i = iLength - 1; i >= 0; --i)
		{
			// aa/bb.exe 9��, 2���ε��� 3 ~ 8������ ����
			if (strAnimPath[i] == '/' || strAnimPath[i] == '\\')
			{
				memset(&strAnimPath[i + 1], 0, sizeof(TCHAR) * (iLength - (i + 1)));
				break;
			}
		}

		char	strAnimFullPath[MAX_PATH] = {};
		strcpy_s(strAnimFullPath, strAnimPath);
		strcat_s(strAnimFullPath, strAnimName);
		strcat_s(strAnimFullPath, ".sqc");

		SaveFullPathMultibyte(strAnimFullPath);
	}

	return true;
}

bool CAnimationSequence::CreateSequenceConvertFBX(bool bLoop, _tagFbxAnimationClip* pClip)
{
	// ���ڷ� ���� �ִϸ��̼� �ɼ������� �����Ѵ�.
	m_Loop = bLoop;
	SetName(pClip->strName);

	m_ChangeFrame = 0;

	switch (pClip->eTimeMode)
	{
	case FbxTime::eFrames24:
		m_FrameMode = 24;
		break;
	case FbxTime::eFrames30:
		m_FrameMode = 30;
		break;
	case FbxTime::eFrames50:
		m_FrameMode = 50;
		break;
	case FbxTime::eFrames60:
		m_FrameMode = 60;
		break;
	}

	// FBXANIMATIONCLIP�� �ִ� starttime �� endtime �� �̿��Ͽ� keyframe �� ���´�.
	m_StartFrame = (int)pClip->tStart.GetFrameCount(pClip->eTimeMode);
	m_EndFrame = (int)pClip->tEnd.GetFrameCount(pClip->eTimeMode);
	m_FrameLength = m_EndFrame - m_StartFrame + 1;

	// (OBJ) => FrameMode, Frame ���� �� ����Ͽ� PlayTime �ٽ� ����
	m_PlayTime = (float)(m_EndFrame - m_StartFrame + 1) / (float)m_FrameMode;

	// �ð� ������ �������ش�.
	m_StartTime = 0.f;
	m_EndTime = m_PlayTime;
	m_TimeLength = m_PlayTime;

	m_FrameTime = m_PlayTime / m_FrameLength;

	std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(pClip->vecBoneKeyFrame.size() * m_FrameLength);

	// �� 3�� Ű������ 5���� ���
	// ��0Ű0|��0Ű1|��0Ű2|��0Ű3|��0Ű4|��1Ű0|��1Ű1|��1Ű2|��1Ű3|��1Ű4|��2Ű0|��2Ű1|��2Ű2|��2Ű3|��2Ű4

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t i = 0; i < pClip->vecBoneKeyFrame.size(); ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;

		pBoneKeyFrame->iBoneIndex = pClip->vecBoneKeyFrame[i].iBoneIndex;

		m_vecKeyFrame.push_back(pBoneKeyFrame);

		// �Ʒ����� Ű������ ������ �����Ѵ�.
		pBoneKeyFrame->vecKeyFrame.reserve(pClip->vecBoneKeyFrame[i].vecKeyFrame.size());

		for (size_t j = 0; j < pClip->vecBoneKeyFrame[i].vecKeyFrame.size(); ++j)
		{
			KeyFrame* pKeyFrame = new KeyFrame;

			pKeyFrame->dTime = j * m_FrameTime;

			// ���� ���� Ű �����ӿ� �ش��ϴ� ��� ������ ���´�.
			FbxAMatrix		mat = pClip->vecBoneKeyFrame[i].vecKeyFrame[j].matTransform;

			FbxVector4		vPos, vScale;
			FbxQuaternion	qRot;

			// ��ķκ��� ��ġ, ũ��, ȸ�� ������ ���´�.
			vPos = mat.GetT();
			vScale = mat.GetS();
			qRot = mat.GetQ();

			pKeyFrame->vScale = Vector3((float)vScale.mData[0], (float)vScale.mData[1],
				(float)vScale.mData[2]);
			pKeyFrame->vPos = Vector3((float)vPos.mData[0], (float)vPos.mData[1],
				(float)vPos.mData[2]);
			pKeyFrame->vRot = Vector4((float)qRot.mData[0], (float)qRot.mData[1],
				(float)qRot.mData[2], (float)qRot.mData[3]);

			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

			if (j < m_FrameLength)
			{
				AnimationFrameTrans	tFrame = {};
				tFrame.vScale = Vector4(pKeyFrame->vScale.x, pKeyFrame->vScale.y,
					pKeyFrame->vScale.z, 1.f);
				tFrame.vTranslate = Vector4(pKeyFrame->vPos.x, pKeyFrame->vPos.y,
					pKeyFrame->vPos.z, 1.f);
				tFrame.qRot = pKeyFrame->vRot;

				m_vecFrameTrans[i * m_FrameLength + j] = tFrame;
			}

			else
				int a = 10;
		}
	}

	if (strlen(m_FullPath) != 0)
	{
		char	strAnimName[MAX_PATH] = {};

		_splitpath_s(m_FullPath, 0, 0, 0, 0, strAnimName, MAX_PATH, 0, 0);

		char	strAnimPath[MAX_PATH] = {};
		strcpy_s(strAnimPath, m_FullPath);

		int	iLength = (int)strlen(strAnimPath);
		for (int i = iLength - 1; i >= 0; --i)
		{
			// aa/bb.exe 9��, 2���ε��� 3 ~ 8������ ����
			if (strAnimPath[i] == '/' || strAnimPath[i] == '\\')
			{
				memset(&strAnimPath[i + 1], 0, sizeof(TCHAR) * (iLength - (i + 1)));
				break;
			}
		}

		char	strAnimFullPath[MAX_PATH] = {};
		strcpy_s(strAnimFullPath, strAnimPath);
		strcat_s(strAnimFullPath, strAnimName);
		strcat_s(strAnimFullPath, ".sqc");

		SaveFullPathMultibyte(strAnimFullPath);
	}

	return true;
}

bool CAnimationSequence::CreateNewSequenceFromExistingSequence(CAnimationSequence* ExistingSequence,
	int StartFrame, int EndFrame, const std::string& NewName)
{
	m_ChangeFrame = 0;
	SetName(NewName);

	switch (ExistingSequence->m_FrameMode)
	{
	case 24:
		m_FrameMode = 24;
		break;
	case 30:
		m_FrameMode = 30;
		break;
	case 50:
		m_FrameMode = 50;
		break;
	case 60:
		m_FrameMode = 60;
		break;
	}

	// ���� ��� ���� 0 ~ 100 
	// �׷��� 10 ~ 20 ������ �ڸ��� �ʹ�.
	// 10 => 0
	// 20 => 10

	m_StartFrame = 0;
	m_EndFrame = EndFrame - StartFrame;
	m_FrameLength = m_EndFrame - m_StartFrame + 1;

	// �ð� ������ �������ش�.
	m_StartTime = 0.f;

	// (OBJ) �ڵ� ���� �߰�
	// �ش� Sequence �� Frame Mode, Frame ������ ����Ͽ� Play Time �ٽ� ����
	// ex. �� 100 Frame => 50FrameMode => �� 2�ʰ� Play
	m_PlayTime = (float)(m_EndFrame - m_StartFrame + 1) / (float)m_FrameMode;

	m_EndTime = m_PlayTime;
	m_TimeLength = m_PlayTime;

	m_FrameTime = m_PlayTime / m_FrameLength;

	// ���� Seqeunece �� ������ Bone ������ �����Ѵ�.
	// Bone ���� * Edit�ϴ� Frame ����
	std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(ExistingSequence->m_vecKeyFrame.size() * m_FrameLength);

	// �� 3�� Ű������ 5���� ���
	// ��0Ű0|��0Ű1|��0Ű2|��0Ű3|��0Ű4|��1Ű0|��1Ű1|��1Ű2|��1Ű3|��1Ű4|��2Ű0|��2Ű1|��2Ű2|��2Ű3|��2Ű4

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t BoneIdx = 0; BoneIdx < ExistingSequence->m_vecKeyFrame.size(); ++BoneIdx)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;

		// pBoneKeyFrame->iBoneIndex = ExistingSequence->m_vecKeyFrame[i].iBoneIndex;
		pBoneKeyFrame->iBoneIndex = ExistingSequence->m_vecKeyFrame[BoneIdx]->iBoneIndex;

		m_vecKeyFrame.push_back(pBoneKeyFrame);

		// �Ʒ����� Ű������ ������ ����
		// ��, �߶� Ű ������ �����鸸�� ������ ���̴�.
		// pBoneKeyFrame->vecKeyFrame.reserve(pClip->vecBoneKeyFrame[i].vecKeyFrame.size());
		pBoneKeyFrame->vecKeyFrame.reserve(m_FrameLength);

		// StartFrame ����, EndFrame ����
		for (size_t KeyFrameIndex = StartFrame; KeyFrameIndex <= EndFrame; ++KeyFrameIndex)
		{
			// �⺻ ������ζ��, 100 Frame ¥�� Animation�̶�� �Ҷ�
			// �� Bone �� ��ΰ� 100�� Frame �� ���ϰ� �־�� �Ѵ�.
			// ������ ���� �׷��� ���� Bone���� ����ִ�.
			if (ExistingSequence->m_vecKeyFrame[BoneIdx]->vecKeyFrame.size() <= KeyFrameIndex)
				break;

			KeyFrame* NewKeyFrame = new KeyFrame;
			NewKeyFrame->dTime = ExistingSequence->m_vecKeyFrame[BoneIdx]->vecKeyFrame[KeyFrameIndex]->dTime;
			NewKeyFrame->vPos = ExistingSequence->m_vecKeyFrame[BoneIdx]->vecKeyFrame[KeyFrameIndex]->vPos;
			NewKeyFrame->vScale = ExistingSequence->m_vecKeyFrame[BoneIdx]->vecKeyFrame[KeyFrameIndex]->vScale;
			NewKeyFrame->vRot = ExistingSequence->m_vecKeyFrame[BoneIdx]->vecKeyFrame[KeyFrameIndex]->vRot;

			// i��° Bone �� j��° Frame ������ ä���ش�.
			// pBoneKeyFrame->vecKeyFrame.push_back(NewKeyFrame);
			pBoneKeyFrame->vecKeyFrame.push_back(NewKeyFrame);

			if (KeyFrameIndex - StartFrame < m_FrameLength)
			{
				AnimationFrameTrans	tFrame = {};
				tFrame.vScale = Vector4(NewKeyFrame->vScale.x, NewKeyFrame->vScale.y,
					NewKeyFrame->vScale.z, 1.f);
				tFrame.vTranslate = Vector4(NewKeyFrame->vPos.x, NewKeyFrame->vPos.y,
					NewKeyFrame->vPos.z, 1.f);
				tFrame.qRot = NewKeyFrame->vRot;

				m_vecFrameTrans[BoneIdx * m_FrameLength + (KeyFrameIndex - StartFrame)] = tFrame;
			}

			else
				int a = 10; // ���� �ɸ��� �ȵȴ�.
		}
	}

	m_KeyFrameBuffer = new CStructuredBuffer;

	m_KeyFrameBuffer->Init("KeyFrameBuffer", (unsigned int)sizeof(AnimationFrameTrans),
		(unsigned int)m_vecFrameTrans.size(), 13, true, (int)Buffer_Shader_Type::Compute);

	m_KeyFrameBuffer->UpdateBuffer(&m_vecFrameTrans[0],
		(int)m_vecFrameTrans.size());

	return true;
}

bool CAnimationSequence::CreateSequence(
	const std::string& strName, bool bLoop, int iStartFrame,
	int iEndFrame, float fPlayTime,
	const std::vector<BoneKeyFrame*>& vecFrame)
{
	// ���ڷ� ���� �ִϸ��̼� �ɼ������� �����Ѵ�.
	m_Loop = bLoop;
	SetName(strName);

	m_ChangeFrame = 0;

	// FBXANIMATIONCLIP�� �ִ� starttime �� endtime �� �̿��Ͽ� keyframe �� ���´�.
	m_StartFrame = 0;
	m_EndFrame = iEndFrame - iStartFrame;
	m_FrameLength = m_EndFrame - m_StartFrame + 1;

	m_PlayTime = fPlayTime;
	m_FrameTime = fPlayTime / m_FrameLength;

	// �ð� ������ �������ش�.
	m_StartTime = 0.f;
	m_EndTime = fPlayTime;
	m_TimeLength = fPlayTime;

	std::vector<AnimationFrameTrans>	m_vecFrameTrans;
	m_vecFrameTrans.resize(vecFrame.size() * m_FrameLength);

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ��������
	// �̾ƿ´�.
	for (size_t i = 0; i < vecFrame.size(); ++i)
	{
		BoneKeyFrame* pBoneKeyFrame = new BoneKeyFrame;

		pBoneKeyFrame->iBoneIndex = vecFrame[i]->iBoneIndex;

		m_vecKeyFrame.push_back(pBoneKeyFrame);

		// �Ʒ����� Ű������ ������ �����Ѵ�.
		pBoneKeyFrame->vecKeyFrame.reserve(m_FrameLength);

		if (!vecFrame[i]->vecKeyFrame.empty())
		{
			for (size_t j = 0; j < m_FrameLength; ++j)
			{
				KeyFrame* pKeyFrame = new KeyFrame;

				pKeyFrame->dTime = (j - iStartFrame) * m_FrameTime;
				pKeyFrame->vScale = vecFrame[i]->vecKeyFrame[j]->vScale;
				pKeyFrame->vRot = vecFrame[i]->vecKeyFrame[j]->vRot;
				pKeyFrame->vPos = vecFrame[i]->vecKeyFrame[j]->vPos;

				pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

				if (j < m_FrameLength)
				{
					AnimationFrameTrans	tFrame = {};
					tFrame.vScale = Vector4(pKeyFrame->vScale.x, pKeyFrame->vScale.y,
						pKeyFrame->vScale.z, 1.f);
					tFrame.vTranslate = Vector4(pKeyFrame->vPos.x, pKeyFrame->vPos.y,
						pKeyFrame->vPos.z, 1.f);
					tFrame.qRot = pKeyFrame->vRot;

					m_vecFrameTrans[i * m_FrameLength + j] = tFrame;
				}

				else
					int a = 10;
			}
		}
	}

	m_KeyFrameBuffer = new CStructuredBuffer;

	m_KeyFrameBuffer->Init("KeyFrameBuffer", (unsigned int)sizeof(AnimationFrameTrans),
		(unsigned int)m_vecFrameTrans.size(), 13, true, (int)Buffer_Shader_Type::Compute);

	m_KeyFrameBuffer->UpdateBuffer(&m_vecFrameTrans[0],
		(unsigned int)m_vecFrameTrans.size());

	return true;
}

bool CAnimationSequence::CreateSequence(const TCHAR* pFullPath)
{
	char	strFullPath[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFullPath, lstrlen(pFullPath),
		0, 0);
#else
	strcpy_s(strFullPath, pFullPath);
#endif // UNICODE

	return CreateSequenceMultibyte(strFullPath);
}

bool CAnimationSequence::CreateSequenceMultibyte(const char* pFullPath)
{
	char	strExt[_MAX_EXT] = {};

	_splitpath_s(pFullPath, 0, 0, 0, 0, 0, 0, strExt, _MAX_EXT);

	_strupr_s(strExt);

	if (strcmp(strExt, ".FBX") == 0)
	{
		return LoadFbxAnimation(pFullPath, m_Loop);
	}

	return LoadFullPathMultibyte(pFullPath);
}

bool CAnimationSequence::CreateSequence(bool bLoop, const std::string& strName,
	const TCHAR* pFileName, const std::string& strPathName)
{
	SetName(strName);

	m_Loop = bLoop;

	const PathInfo* pPath = CPathManager::GetInst()->FindPath(strPathName);

	TCHAR	strFullPath[MAX_PATH] = {};

	if (pPath)
		lstrcpy(strFullPath, pPath->Path);

	lstrcat(strFullPath, pFileName);

	return CreateSequence(strFullPath);
}

void CAnimationSequence::SetShader()
{
	m_KeyFrameBuffer->SetShader();
}

void CAnimationSequence::ResetShader()
{
	m_KeyFrameBuffer->ResetShader();
}

void CAnimationSequence::SetChangeShader()
{
	m_KeyFrameBuffer->SetShader(15, (int)Buffer_Shader_Type::Compute);
}

void CAnimationSequence::ResetChangeShader()
{
	m_KeyFrameBuffer->ResetShader(15, (int)Buffer_Shader_Type::Compute);
}

void CAnimationSequence::Clear()
{
}
