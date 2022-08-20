#pragma once

#include "../GameInfo.h"


struct DVector3
{
	double x, y, z;


	DVector3() :
		x(0.f),
		y(0.f),
		z(0.f)
	{}

	DVector3(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

class CCollision
{
public:
	static bool CollisionBox2DToBox2D(class CColliderBox2D* Src, class CColliderBox2D* Dest);
	static bool CollisionCircleToCircle(class CColliderCircle* Src, class CColliderCircle* Dest);
	static bool CollisionBox2DToCircle(class CColliderBox2D* Src, class CColliderCircle* Dest);
	static bool CollisionBox2DToPixel(class CColliderBox2D* Src, class CColliderPixel* Dest);
	static bool CollisionCircleToPixel(class CColliderCircle* Src, class CColliderPixel* Dest);
	static bool CollisionBox3DToSphere(class CColliderBox3D* Src, class CColliderSphere* Dest);
	static bool CollisionBox3DToBox3D(class CColliderBox3D* Src, class CColliderBox3D* Dest);
	static bool CollisionRayToBox3D(class CColliderRay* Src, class CColliderBox3D* Dest);
	static bool CollisionBox3DToHalfLine(class CColliderBox3D* Src, class CColliderHalfLine* Dest);
	static bool CollisionSphereToSphere(class CColliderSphere* Src, class CColliderSphere* Dest);


public:
	static bool CollisionBox2DToBox2D(CollisionResult& SrcResult, CollisionResult& DestResult,
		const Box2DInfo& Src, const Box2DInfo& Dest);
	static bool CollisionCircleToCircle(CollisionResult& SrcResult, CollisionResult& DestResult,
		const CircleInfo& Src, const CircleInfo& Dest);
	static bool CollisionBox2DToCircle(CollisionResult& SrcResult, CollisionResult& DestResult,
		const Box2DInfo& Src, const CircleInfo& Dest);
	static bool CollisionBox2DToPixel(CollisionResult& SrcResult, CollisionResult& DestResult,
		const Box2DInfo& Src, const PixelInfo& Dest);
	static bool CollisionCircleToPixel(CollisionResult& SrcResult, CollisionResult& DestResult,
		const CircleInfo& Src, const PixelInfo& Dest);


	static bool CollisionBox2DToPoint(CollisionResult& SrcResult, CollisionResult& DestResult,
		const Box2DInfo& BoxInfo, const Vector2& Point);
	static bool CollisionCircleToPoint(CollisionResult& SrcResult, CollisionResult& DestResult,
		const CircleInfo& CircleInfo, const Vector2& Point);
	static bool CollisionPixelToPoint(CollisionResult& SrcResult, CollisionResult& DestResult,
		const PixelInfo& PixelInfo, const Vector2& Point);
	static bool CollisionBox3DToBox3D(CollisionResult& SrcResult, CollisionResult& DestResult,
		const Box3DInfo& Src, const Box3DInfo& Dest);
	static bool CollisionSphereToSphere(CollisionResult& SrcResult, CollisionResult& DestResult,
		const SphereInfo& SrcInfo, const SphereInfo& DestInfo);

	static bool CollisionRayToSphere(Vector3& HitPoint, const Ray& ray, const SphereInfo& Sphere);
	static bool CollisionRayToBox3D(float& Scale, const Ray& ray, const Box3DInfo& Box);
	static bool CollisionBox3DToHalfLine(float& Scale, const Box3DInfo& Box, const HalfLineInfo& HalfLineInfo, class CColliderBox3D* BoxCollider);
	static bool CollisionBox3DToSphere(Vector3& Scale, class CColliderBox3D* Src, const Box3DInfo& Box, const SphereInfo& Sphere);



	static bool CheckPointInBox(const Vector3& Point, const Box3DInfo& Box);
};

