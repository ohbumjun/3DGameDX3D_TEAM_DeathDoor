
#include "AnimationSequenceData.h"
#include "../Resource/Animation/AnimationSequence.h"
#include "../Resource/ResourceManager.h"

CAnimationSequenceData::CAnimationSequenceData() :
	m_Time(0.f),
	m_FrameTime(0.f),
	m_PlayTime(0.f),
	m_PlayScale(1.f),
	m_Loop(0),
	m_EndFunctionCalled(false)
{
}

CAnimationSequenceData::~CAnimationSequenceData()
{
	auto	iter = m_vecNotify.begin();
	auto	iterEnd = m_vecNotify.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}
}

void CAnimationSequenceData::Save(FILE* File)
{
	int	Length = (int)m_Name.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_Name.c_str(), sizeof(char), Length, File);


	fwrite(&m_Time, sizeof(float), 1, File);
	fwrite(&m_FrameTime, sizeof(float), 1, File);
	fwrite(&m_PlayTime, sizeof(float), 1, File);
	fwrite(&m_PlayScale, sizeof(float), 1, File);

	fwrite(&m_Loop, sizeof(bool), 1, File);

	std::string	SequenceName = m_Sequence->GetName();

	Length = (int)SequenceName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(SequenceName.c_str(), sizeof(char), Length, File);

	m_Sequence->Save(File);
}

void CAnimationSequenceData::Load(FILE* File)
{
	int	Length = 0;
	char	Name[256] = {};

	fread(&Length, sizeof(int), 1, File);
	fread(Name, sizeof(char), Length, File);
	m_Name = Name;

	fread(&m_Time, sizeof(float), 1, File);
	fread(&m_FrameTime, sizeof(float), 1, File);
	fread(&m_PlayTime, sizeof(float), 1, File);
	fread(&m_PlayScale, sizeof(float), 1, File);
	fread(&m_Loop, sizeof(bool), 1, File);

	Length = 0;
	char	SequenceName[256] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(SequenceName, sizeof(char), Length, File);

	m_SequenceName = SequenceName;

	m_Sequence = CResourceManager::GetInst()->FindAnimationSequence(m_SequenceName);

	// 존재하지 않는 Sequence 라면, 새롭게 ResourceManager 항목에 추가해주고 Load
	if (!m_Sequence)
	{
		m_Sequence = CResourceManager::GetInst()->CreateBasicAnimationSequence(m_SequenceName);
		m_Sequence->Load(File);
	}
	// 이미 존재한다면, Load 순서상 Load 만 해주고 파기
	else
	{
		CAnimationSequence* TempSequence = new CAnimationSequence;
		TempSequence->DummyLoad(File);
		SAFE_DELETE(TempSequence)
	}

}

void CAnimationSequenceData::SetPlayTime(float Time)
{
	m_PlayTime = Time;

	m_FrameTime = m_PlayTime / m_Sequence->GetFrameLength();
}

void CAnimationSequenceData::SetOriginalFramePlayTime()
{
	int StartFrame = m_Sequence->GetStartFrame();
	int EndFrame = m_Sequence->GetEndFrame();
	int FrameMode = m_Sequence->GetFrameMode();

	float OriginalPlayTime = (float)(EndFrame - StartFrame + 1) / (float)FrameMode;

	m_PlayTime = OriginalPlayTime;
	m_Sequence->m_PlayTime = OriginalPlayTime;

	m_FrameTime = m_PlayTime / m_Sequence->GetFrameLength();
	m_Sequence->m_FrameTime = m_PlayTime / m_Sequence->GetFrameLength();
}
