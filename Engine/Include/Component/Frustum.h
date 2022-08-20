#pragma once

#include "../GameInfo.h"

enum class Frustum_Plane_Dir
{
	Left,
	Right,
	Bottom,
	Top,
	Near,
	Far,
	Max
};

class CFrustum
{
	friend class CCameraComponent;

private:
	CFrustum();
	~CFrustum();
public:
	void Update(Matrix MatVP);
	bool FrustumInPoint(const Vector3& Point);
	bool FrustumInSphere(const SphereInfo& Sphere);

private:
	Vector3 m_Pos[8];
	Vector4 m_Plane[(int)Frustum_Plane_Dir::Max];
};

