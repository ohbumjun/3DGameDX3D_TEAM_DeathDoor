#pragma once

#include "IMGUIWidget.h"

class CIMGUIInputFloat3 :
	public CIMGUIWidget
{
public:
	CIMGUIInputFloat3();
	virtual ~CIMGUIInputFloat3();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetVal(const Vector3& value)
	{
		mValue = value;
	}

	void SetVal(const float x, const float y, const float z)
	{
		mValue = Vector3(x, y, z);
	}

	void SetX(const float x)
	{
		mValue.x = x;
	}

	void SetY(const float y)
	{
		mValue.y = y;
	}

	void SetZ(const float z)
	{
		mValue.z = z;
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

	float GetZ() const
	{
		return mValue.z;
	}

	const Vector3& GetValue() const
	{
		return mValue;
	}

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(const Vector3&))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	Vector3 mValue;
	std::function<void(const Vector3&)> mCallBack;
	bool mbInputRange;
	float mMin;
	float mMax;
};


