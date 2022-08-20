#pragma once

#include "IMGUIWidget.h"

class CIMGUISliderInt :
    public CIMGUIWidget
{
public:
	CIMGUISliderInt();
	virtual ~CIMGUISliderInt();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	int GetValue()	const
	{
		return mValue;
	}

public:
	void SetValue(const int value)
	{
		mValue = value;
	}

	void SetMin(const int value)
	{
		mMin = value;
	}

	void SetMax(const int value)
	{
		mMax = value;
	}

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(int))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	int mValue;
	int mMin;
	int mMax;
	std::function<void(int)> mCallBack;
};

