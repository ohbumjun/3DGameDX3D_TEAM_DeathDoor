#pragma once

#include "IMGUIWidget.h"

class CIMGUIInputInt2 :
    public CIMGUIWidget
{
public:
	CIMGUIInputInt2();
	virtual ~CIMGUIInputInt2();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetVal(const Vector2& val)
	{
		mValue[0] = (int)val.x;
		mValue[1] = (int)val.y;
	}

	void SetVal(const int x, const int y)
	{
		mValue[0] = x;
		mValue[1] = y;
	}

	void SetX(const int x)
	{
		mValue[0] = x;
	}

	void SetY(const int y)
	{
		mValue[1] = y;
	}

	void EnableInputRange(bool bEnable)
	{
		mbInputRange = bEnable;
	}

	void SetMin(const int val)
	{
		mMin = val;
	}
	
	void SetMax(const int val)
	{
		mMax = val;
	}

public:
	int GetX() const
	{
		return mValue[0];
	}

	int GetY() const
	{
		return mValue[1];
	}

	Vector2 GetValue() const
	{
		return Vector2((float)mValue[0], (float)mValue[1]);
	}

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(int[2]))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	int mValue[2];
	std::function<void(int[2])> mCallBack;
	bool mbInputRange; // 입력 범위가 존재하는지
	int mMin;
	int mMax;
};

