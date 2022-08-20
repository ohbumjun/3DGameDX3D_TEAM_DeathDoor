#pragma once
#include "IMGUIWidget.h"
class CIMGUIColor3 :
	public CIMGUIWidget
{
public:
	CIMGUIColor3();
	virtual ~CIMGUIColor3();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetRGB(const Vector3& color)
	{
		mValue = color;
	}

	void SetRGB(const float r, const float g, const float b)
	{
		mValue = Vector3(r, g, b);
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

public:
	template <typename T>
	void SetCallBack(T* obj, void(T::* func)(const Vector3&))
	{
		mCallBack = std::bind(func, obj, std::placeholders::_1);
	}

protected:
	Vector3 mValue;
	std::function<void(const Vector3&)> mCallBack;
};



