#pragma once

#include "../GameInfo.h"
#include "../Resource/Animation/AnimationSequence.h"

struct AnimationNotify
{
	std::string	Name;
	int		Frame;
	bool	Call;
	std::function<void()>	Function;
	std::function<void(float)>	FunctionDetlaTime;

	AnimationNotify() :
		Frame(0),
		Call(false),
		Function(nullptr)
	{
	}
};

class CAnimationSequenceData
{
	friend class CAnimationSequenceInstance;

private:
	CAnimationSequenceData();
	~CAnimationSequenceData();

private:
	std::string	m_Name;
	std::string	m_SequenceName;
	CSharedPtr<class CAnimationSequence>	m_Sequence;
	float	m_Time;			// 애니메이션 동작 시간
	float	m_FrameTime;	// 1프레임당 시간
	float	m_PlayTime;
	float	m_PlayScale;		// 재생 비율
	bool	m_Loop;
	bool	m_EndFunctionCalled;
	std::function<void()>	m_EndFunction;
	std::vector<AnimationNotify*>	m_vecNotify;
public:
	void Save(FILE* File);
	void Load(FILE* File);
public:
	void SetPlayTime(float Time);
	void SetPlayScale(float Scale)
	{
		m_PlayScale = Scale;
	}
	void SetOriginalFramePlayTime();
public:
	bool IsLoop() const
	{
		return m_Loop;
	}

	const std::string& GetName()	const
	{
		return m_Name;
	}

	float GetAnimationTime()	const
	{
		return m_Time;
	}

	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	class CAnimationSequence* GetAnimationSequence()	const
	{
		return m_Sequence;
	}

	float GetAnimationPlayScale()   const
	{
		return m_PlayScale;
	}

	float GetAnimationPlayTime()   const
	{
		return m_PlayTime;
	}

public:
	template <typename T>
	void SetEndFunction(T* Obj, void (T::* Func)())
	{
		m_EndFunction = std::bind(Func, Obj);
	}

	template <typename T>
	void AddNotify(const std::string& Name, int Frame, T* Obj, void (T::* Func)())
	{
		AnimationNotify* Notify = new AnimationNotify;

		Notify->Name = Name;
		Notify->Frame = Frame;
		Notify->Function = std::bind(Func, Obj);

		m_vecNotify.push_back(Notify);
	}

	template <typename T>
	void AddNotifyDetlaTime(const std::string& Name, int Frame, T* Obj, void (T::* Func)(float))
	{
		AnimationNotify* Notify = new AnimationNotify;

		Notify->Name = Name;
		Notify->Frame = Frame;
		Notify->FunctionDetlaTime = std::bind(Func, Obj, std::placeholders::_1);

		m_vecNotify.push_back(Notify);
	}

	template <typename T>
	void AddNotifyFrameRange(const std::string& Name, int FrameStart, int FrameEnd,
		T* Obj, void(T::* Func)())
	{
		int StartFrame = FrameStart;
		int EndFrame = FrameEnd;

		if (StartFrame < 0)
		{
			StartFrame == 0;
		}

		int SeqEndFrame = m_Sequence->GetEndFrame();
		if (EndFrame > SeqEndFrame)
		{
			EndFrame = SeqEndFrame;
		}

		for (int i = StartFrame; i <= EndFrame; ++i)
		{
			AnimationNotify* Notify = new AnimationNotify;
			Notify->Name = Name;
			Notify->Frame = i;
			Notify->Function = std::bind(Func, Obj);
			
			m_vecNotify.push_back(Notify);
		}
	}

	template <typename T>
	void AddNotifyDeltaTimeFrameRange(const std::string& Name, int FrameStart, int FrameEnd,
		T* Obj, void(T::* Func)(float))
	{
		int StartFrame = FrameStart;
		int EndFrame = FrameEnd;

		if (StartFrame < 0)
		{
			StartFrame = 0;
		}

		int SeqEndFrame = m_Sequence->GetEndFrame();
		if (EndFrame > SeqEndFrame)
		{
			EndFrame = SeqEndFrame;
		}

		for (int i = StartFrame; i <= EndFrame; ++i)
		{
			AnimationNotify* Notify = new AnimationNotify;
			Notify->Name = Name;
			Notify->Frame = i;
			Notify->FunctionDetlaTime = std::bind(Func, Obj, std::placeholders::_1);
			
			m_vecNotify.push_back(Notify);
		}
	}
};

