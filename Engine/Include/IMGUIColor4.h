#pragma once

#include "IMGUIWidget.h"

class CIMGUIColor4 :
	public CIMGUIWidget
{
public:
	CIMGUIColor4();
	virtual ~CIMGUIColor4();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetRGBA(const Vector4& color)
	{
		mValue = color;
	}

	void SetRGBA(const float r, const float g, const float b, const float a)
	{
		mValue = Vector4(r, g, b, a);
	}

	void SetR(const float r)
	{
		mValue.x = r;
	}

	void SetG(const float g)
	{
		mValue.y = g;
	}

	void SetB(const float b)
	{
		mValue.z = b;
	}

	void SetA(const float a)
	{
		mValue.w = a;
	}

public:
	float GetR() const
	{
		return mValue.x;
	}

	float GetG() const
	{
		return mValue.y;
	}

	float GetB() const
	{
		return mValue.z;
	}

	float GetA() const
	{
		return mValue.w;
	}

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(const Vector4&))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	Vector4 mValue;
	std::function<void(const Vector4&)> mCallBack;
};

