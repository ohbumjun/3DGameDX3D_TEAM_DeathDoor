#pragma once

#include "IMGUIWidget.h"

class CIMGUIInputFloat2 :
	public CIMGUIWidget
{
public:
	CIMGUIInputFloat2();
	virtual ~CIMGUIInputFloat2();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetVal(const Vector2& value)
	{
		mValue = value;
	}

	void SetVal(const float x, const float y)
	{
		mValue = Vector2(x, y);
	}

	void SetX(const float x)
	{
		mValue.x = x;
	}

	void SetY(const float y)
	{
		mValue.y = y;
	}

	void EnableInputRange(bool bEnable)
	{
		mbInputRange = bEnable;
	}

	void SetMin(const float val)
	{
		mMin = val;
	}

	void SetMax(const float val)
	{
		mMax = val;
	}

public:
	float GetX() const
	{
		return mValue.x;
	}

	float GetY() const
	{
		return mValue.y;
	}

	const Vector2& GetValue() const
	{
		return mValue;
	}

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(const Vector2&))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	Vector2 mValue;
	std::function<void(const Vector2&)> mCallBack;
	bool mbInputRange;
	float mMin;
	float mMax;
};


