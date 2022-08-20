#pragma once

#include "IMGUIWidget.h"

class CIMGUISliderFloat :
    public CIMGUIWidget
{
public:
	CIMGUISliderFloat();
	virtual ~CIMGUISliderFloat();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	float GetValue()	const
	{
		return mValue;
	}

public:
	void SetValue(const float value)
	{
		mValue = value;
	}

	void SetMin(const float value)
	{
		mMin = value;
	}

	void SetMax(const float value)
	{
		mMax = value;
	}

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(float))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	float mValue;
	float mMin;
	float mMax;
	std::function<void(float)> mCallBack;
};

