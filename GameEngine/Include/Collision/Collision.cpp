#include "Collision.h"
#include "../Component/ColliderBox2D.h"
#include "../Component/ColliderCircle.h"
#include "../Component/ColliderPixel.h"
#include "../Component/ColliderBox3D.h"
#include "../Component/ColliderSphere.h"
#include "../Component/ColliderHalfLine.h"
#include "../Component/ColliderRay.h"

#define EPSILON 0.00001f


bool CCollision::CollisionBox2DToBox2D(CColliderBox2D* Src, CColliderBox2D* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionBox2DToBox2D(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		srcResult.Src = Src;
		srcResult.Dest = Dest;

		destResult.Src = Dest;
		destResult.Dest = Src;

		Src->m_Result = srcResult;
		Dest->m_Result = destResult;

		return true;
	}

	return false;
}

bool CCollision::CollisionCircleToCircle(CColliderCircle* Src, CColliderCircle* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionCircleToCircle(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		srcResult.Src = Src;
		srcResult.Dest = Dest;

		destResult.Src = Dest;
		destResult.Dest = Src;

		Src->m_Result = srcResult;
		Dest->m_Result = destResult;

		return true;
	}

	return false;
}

bool CCollision::CollisionBox2DToCircle(CColliderBox2D* Src, CColliderCircle* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionBox2DToCircle(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		srcResult.Src = Src;
		srcResult.Dest = Dest;

		destResult.Src = Dest;
		destResult.Dest = Src;

		Src->m_Result = srcResult;
		Dest->m_Result = destResult;

		return true;
	}

	return false;
}

bool CCollision::CollisionBox2DToPixel(CColliderBox2D* Src, CColliderPixel* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionBox2DToPixel(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		srcResult.Src = Src;
		srcResult.Dest = Dest;

		destResult.Src = Dest;
		destResult.Dest = Src;

		Src->m_Result = srcResult;
		Dest->m_Result = destResult;

		return true;
	}

	return false;
}

bool CCollision::CollisionCircleToPixel(CColliderCircle* Src, CColliderPixel* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionCircleToPixel(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		srcResult.Src = Src;
		srcResult.Dest = Dest;

		destResult.Src = Dest;
		destResult.Dest = Src;

		Src->m_Result = srcResult;
		Dest->m_Result = destResult;

		return true;
	}

	return false;
}

bool CCollision::CollisionBox3DToSphere(CColliderBox3D* Src, CColliderSphere* Dest)
{
	Vector3 HitPoint;

	if (CollisionBox3DToSphere(HitPoint, Src, Src->GetInfo(), Dest->GetInfo()))
	{
		return true;
	}

	return false;
}

bool CCollision::CollisionBox3DToBox3D(CColliderBox3D* Src, CColliderBox3D* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionBox3DToBox3D(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		srcResult.Src = Src;
		srcResult.Dest = Dest;

		destResult.Src = Dest;
		destResult.Dest = Src;

		Src->m_Result = srcResult;
		Dest->m_Result = destResult;


		return true;
	}



	return false;
}

bool CCollision::CollisionRayToBox3D(CColliderRay* Src, CColliderBox3D* Dest)
{
	float HitPointDist = -1.f;

	if (CollisionRayToBox3D(HitPointDist, Src->GetInfo(), Dest->GetInfo()))
	{
		return true;
	}

	return false;
}

bool CCollision::CollisionBox3DToHalfLine(CColliderBox3D* Src, CColliderHalfLine* Dest)
{
	float Scale = -1.f;

	Src->UpdateMinMax();

	if (CollisionBox3DToHalfLine(Scale, Src->GetInfo(), Dest->GetInfo(), Src))
	{

		Dest->SetWorldScale(Scale + 0.05f, 1.f, 1.f);

		return true;
	}

	return false;
}

bool CCollision::CollisionSphereToSphere(CColliderSphere* Src, CColliderSphere* Dest)
{
	CollisionResult	srcResult, destResult;

	if (CollisionSphereToSphere(srcResult, destResult, Src->GetInfo(), Dest->GetInfo()))
	{
		Src->SetCollisionResultHitPoint(srcResult.HitPoint);
		Dest->SetCollisionResultHitPoint(srcResult.HitPoint);

		return true;
	}

	return false;
}

bool CCollision::CollisionBox2DToBox2D(CollisionResult& SrcResult,
	CollisionResult& DestResult, const Box2DInfo& Src, const Box2DInfo& Dest)
{
	Vector2	CenterDir = Src.Center - Dest.Center;

	Vector2	Axis = Src.Axis[0];

	float	CenterProjDist = abs(CenterDir.Dot(Axis));

	float	r1, r2;

	r1 = Src.Length.x;
	r2 = abs(Dest.Axis[0].Dot(Axis) * Dest.Length.x) +
		abs(Dest.Axis[1].Dot(Axis) * Dest.Length.y);

	if (CenterProjDist > r1 + r2)
		return false;

	Axis = Src.Axis[1];

	CenterProjDist = abs(CenterDir.Dot(Axis));

	r1 = Src.Length.y;
	r2 = abs(Dest.Axis[0].Dot(Axis) * Dest.Length.x) +
		abs(Dest.Axis[1].Dot(Axis) * Dest.Length.y);

	if (CenterProjDist > r1 + r2)
		return false;

	Axis = Dest.Axis[0];

	CenterProjDist = abs(CenterDir.Dot(Axis));

	r1 = Dest.Length.x;
	r2 = abs(Src.Axis[0].Dot(Axis) * Src.Length.x) +
		abs(Src.Axis[1].Dot(Axis) * Src.Length.y);

	if (CenterProjDist > r1 + r2)
		return false;

	Axis = Dest.Axis[1];

	CenterProjDist = abs(CenterDir.Dot(Axis));

	r1 = Dest.Length.y;
	r2 = abs(Src.Axis[0].Dot(Axis) * Src.Length.x) +
		abs(Src.Axis[1].Dot(Axis) * Src.Length.y);

	if (CenterProjDist > r1 + r2)
		return false;

	return true;
}

bool CCollision::CollisionCircleToCircle(CollisionResult& SrcResult, 
	CollisionResult& DestResult, const CircleInfo& Src, const CircleInfo& Dest)
{
	float	Dist = Src.Center.Distance(Dest.Center);

	return Dist <= Src.Radius + Dest.Radius;
}

bool CCollision::CollisionBox2DToCircle(CollisionResult& SrcResult, 
	CollisionResult& DestResult, const Box2DInfo& Src, const CircleInfo& Dest)
{
	Vector2	CenterDir = Src.Center - Dest.Center;

	Vector2	Axis = Src.Axis[0];

	float	CenterProjDist = abs(CenterDir.Dot(Axis));

	float	r1, r2;

	r1 = Src.Length.x;
	r2 = Dest.Radius;

	if (CenterProjDist > r1 + r2)
		return false;

	Axis = Src.Axis[1];

	CenterProjDist = abs(CenterDir.Dot(Axis));

	r1 = Src.Length.y;
	r2 = Dest.Radius;

	if (CenterProjDist > r1 + r2)
		return false;

	Axis = CenterDir;
	Axis.Normalize();

	CenterProjDist = CenterDir.Length();

	r1 = abs(Src.Axis[0].Dot(Axis) * Src.Length.x) +
		abs(Src.Axis[1].Dot(Axis) * Src.Length.y);
	r2 = Dest.Radius;

	if (CenterProjDist > r1 + r2)
		return false;

	return true;
}

bool CCollision::CollisionBox2DToPixel(CollisionResult& SrcResult, CollisionResult& DestResult, 
	const Box2DInfo& Src, const PixelInfo& Dest)
{
	if (!CollisionBox2DToBox2D(SrcResult, DestResult, Src, Dest.Box))
		return false;

	// 교집합을 구한다.
	float	Left = Src.Min.x < Dest.Min.x ? Dest.Min.x : Src.Min.x;
	float	Right = Src.Max.x > Dest.Max.x ? Dest.Max.x : Src.Max.x;

	float	Bottom = Src.Min.y < Dest.Min.y ? Dest.Min.y : Src.Min.y;
	float	Top = Src.Max.y > Dest.Max.y ? Dest.Max.y : Src.Max.y;

	// 월드 공간에서의 좌 하단 좌표를 구한다.
	Vector2	LB = Dest.Box.Center - Dest.Box.Length;

	Left -= LB.x;
	Right -= LB.x;

	Bottom -= LB.y;
	Top -= LB.y;

	Left = Left < 0.f ? 0.f : Left;
	Bottom = Bottom < 0.f ? 0.f : Bottom;

	Right = Right >= (float)Dest.Width ? (float)Dest.Width - 1.f : Right;
	Top = Top >= (float)Dest.Height ? (float)Dest.Height - 1.f : Top;

	Top = Dest.Height - Top;
	Bottom = Dest.Height - Bottom;

	bool	Collision = false;

	// 교집합 구간을 반복한다.
	for (int y = (int)Top; y < (int)Bottom; ++y)
	{
		for (int x = (int)Left; x < (int)Right; ++x)
		{
			int	Index = y * (int)Dest.Width * 4 + x * 4;

			// 현재 인덱스의 픽셀이 상대방 박스 안에 존재하는지를 판단한다.
			// 현재 픽셀의 월드공간에서의 위치를 구해준다.
			Vector2	PixelWorldPos = LB + Vector2((float)x, (float)Dest.Height - (float)y);
			if (!CollisionBox2DToPoint(SrcResult, DestResult, Src, PixelWorldPos))
				continue;

			switch (Dest.Type)
			{
			case PixelCollision_Type::Color_Ignore:
				if (Dest.Pixel[Index] == Dest.Color[0] &&
					Dest.Pixel[Index + 1] == Dest.Color[1] &&
					Dest.Pixel[Index + 2] == Dest.Color[2])
					continue;

				Collision = true;
				break;
			case PixelCollision_Type::Color_Confirm:
				if (Dest.Pixel[Index] == Dest.Color[0] &&
					Dest.Pixel[Index + 1] == Dest.Color[1] &&
					Dest.Pixel[Index + 2] == Dest.Color[2])
					Collision = true;
				break;
			case PixelCollision_Type::Alpha_Ignore:
				if (Dest.Pixel[Index + 3] == Dest.Color[3])
					continue;

				Collision = true;
				break;
			case PixelCollision_Type::Alpha_Confirm:
				if (Dest.Pixel[Index + 3] == Dest.Color[3])
					Collision = true;
				break;
			}

			if (Collision)
				break;
		}

		if (Collision)
			break;
	}

	return Collision;
}

bool CCollision::CollisionCircleToPixel(CollisionResult& SrcResult, CollisionResult& DestResult,
	const CircleInfo& Src, const PixelInfo& Dest)
{
	if (!CollisionBox2DToCircle(SrcResult, DestResult, Dest.Box, Src))
		return false;

	// 교집합을 구한다.
	float	Left = Src.Min.x < Dest.Min.x ? Dest.Min.x : Src.Min.x;
	float	Right = Src.Max.x > Dest.Max.x ? Dest.Max.x : Src.Max.x;

	float	Bottom = Src.Min.y < Dest.Min.y ? Dest.Min.y : Src.Min.y;
	float	Top = Src.Max.y > Dest.Max.y ? Dest.Max.y : Src.Max.y;

	// 월드 공간에서의 좌 하단 좌표를 구한다.
	Vector2	LB = Dest.Box.Center - Dest.Box.Length;

	Left -= LB.x;
	Right -= LB.x;

	Bottom -= LB.y;
	Top -= LB.y;

	Left = Left < 0.f ? 0.f : Left;
	Bottom = Bottom < 0.f ? 0.f : Bottom;

	Right = Right >= (float)Dest.Width ? (float)Dest.Width - 1.f : Right;
	Top = Top >= (float)Dest.Height ? (float)Dest.Height - 1.f : Top;

	Top = Dest.Height - Top;
	Bottom = Dest.Height - Bottom;

	bool	Collision = false;

	// 교집합 구간을 반복한다.
	for (int y = (int)Top; y < (int)Bottom; ++y)
	{
		for (int x = (int)Left; x < (int)Right; ++x)
		{
			int	Index = y * (int)Dest.Width * 4 + x * 4;

			// 현재 인덱스의 픽셀이 상대방 박스 안에 존재하는지를 판단한다.
			// 현재 픽셀의 월드공간에서의 위치를 구해준다.
			Vector2	PixelWorldPos = LB + Vector2((float)x, (float)Dest.Height - (float)y);
			if (!CollisionCircleToPoint(SrcResult, DestResult, Src, PixelWorldPos))
				continue;

			switch (Dest.Type)
			{
			case PixelCollision_Type::Color_Ignore:
				if (Dest.Pixel[Index] == Dest.Color[0] &&
					Dest.Pixel[Index + 1] == Dest.Color[1] &&
					Dest.Pixel[Index + 2] == Dest.Color[2])
					continue;

				Collision = true;
				break;
			case PixelCollision_Type::Color_Confirm:
				if (Dest.Pixel[Index] == Dest.Color[0] &&
					Dest.Pixel[Index + 1] == Dest.Color[1] &&
					Dest.Pixel[Index + 2] == Dest.Color[2])
					Collision = true;
				break;
			case PixelCollision_Type::Alpha_Ignore:
				if (Dest.Pixel[Index + 3] == Dest.Color[3])
					continue;

				Collision = true;
				break;
			case PixelCollision_Type::Alpha_Confirm:
				if (Dest.Pixel[Index + 3] == Dest.Color[3])
					Collision = true;
				break;
			}

			if (Collision)
				break;
		}

		if (Collision)
			break;
	}

	return Collision;
}

bool CCollision::CollisionBox2DToPoint(CollisionResult& SrcResult,
	CollisionResult& DestResult, const Box2DInfo& BoxInfo, const Vector2& Point)
{
	// 상자의 x, y 축에 점을 투영하여 구간이 겹치는지 판단한다.
	Vector2	CenterDir = BoxInfo.Center - Point;

	Vector2	Axis = BoxInfo.Axis[0];

	float	CenterProjDist = abs(CenterDir.Dot(Axis));

	if (CenterProjDist > BoxInfo.Length.x)
		return false;

	Axis = BoxInfo.Axis[1];

	CenterProjDist = abs(CenterDir.Dot(Axis));

	if (CenterProjDist > BoxInfo.Length.y)
		return false;

	return true;
}

bool CCollision::CollisionCircleToPoint(CollisionResult& SrcResult, 
	CollisionResult& DestResult, const CircleInfo& CircleInfo, const Vector2& Point)
{
	float	Dist = CircleInfo.Center.Distance(Point);

	return CircleInfo.Radius >= Dist;
}

bool CCollision::CollisionPixelToPoint(CollisionResult& SrcResult, CollisionResult& DestResult, 
	const PixelInfo& PixelInfo, const Vector2& Point)
{
	if (!CollisionBox2DToPoint(SrcResult, DestResult, PixelInfo.Box, Point))
		return false;

	Vector2	LB = PixelInfo.Box.Center - PixelInfo.Box.Length;

	Vector2	ConvertPoint = Point - LB;

	ConvertPoint.y = PixelInfo.Height - ConvertPoint.y;

	int	Index = (int)ConvertPoint.y * (int)PixelInfo.Width * 4 + (int)ConvertPoint.x * 4;

	bool	Result = false;

	switch (PixelInfo.Type)
	{
	case PixelCollision_Type::Color_Ignore:
		if (PixelInfo.Pixel[Index] == PixelInfo.Color[0] &&
			PixelInfo.Pixel[Index + 1] == PixelInfo.Color[1] &&
			PixelInfo.Pixel[Index + 2] == PixelInfo.Color[2])
			Result = false;

		else
			Result = true;
		break;
	case PixelCollision_Type::Color_Confirm:
		if (PixelInfo.Pixel[Index] == PixelInfo.Color[0] &&
			PixelInfo.Pixel[Index + 1] == PixelInfo.Color[1] &&
			PixelInfo.Pixel[Index + 2] == PixelInfo.Color[2])
			Result = true;

		else
			Result = false;
		break;
	case PixelCollision_Type::Alpha_Ignore:
		if (PixelInfo.Pixel[Index + 3] == PixelInfo.Color[3])
			Result = false;

		else
			Result = true;
		break;
	case PixelCollision_Type::Alpha_Confirm:
		if (PixelInfo.Pixel[Index + 3] == PixelInfo.Color[3])
			Result = true;

		else
			Result = false;
		break;
	}

	return Result;
}

bool CCollision::CollisionBox3DToBox3D(CollisionResult& SrcResult, CollisionResult& DestResult, const Box3DInfo& boundingBox, const Box3DInfo& targetBox)
{
	double c[3][3];
	double absC[3][3];
	double d[3];
	double r0, r1, r;
	int i;
	const double cutoff = 0.999999;
	bool existsParallelPair = false;
	Vector3 diff = boundingBox.Center - targetBox.Center;

	// 분리축 : Src의 x축
	for (i = 0; i < 3; ++i)
	{
		c[0][i] = boundingBox.Axis[0].Dot(targetBox.Axis[i]);

		absC[0][i] = abs(c[0][i]);

		if (absC[0][i] > cutoff)
			existsParallelPair = true;
	}

	//d[0] = diff.Dot(targetBox.Axis[0]);
	d[0] = diff.Dot(boundingBox.Axis[0]);

	r = abs(d[0]);
	r0 = boundingBox.AxisLen[0];
	r1 = targetBox.AxisLen[0] * absC[0][0] + targetBox.AxisLen[1] * absC[0][1] + targetBox.AxisLen[2] * absC[0][2];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Src의 y축
	for (i = 0; i < 3; ++i)
	{
		c[1][i] = boundingBox.Axis[1].Dot(targetBox.Axis[i]);

		absC[1][i] = abs(c[1][i]);
		if (absC[1][i] > cutoff)
			existsParallelPair = true;
	}

	//d[1] = diff.Dot(targetBox.Axis[1]);
	d[1] = diff.Dot(boundingBox.Axis[1]);

	r = abs(d[1]);
	r0 = boundingBox.AxisLen[1];
	r1 = targetBox.AxisLen[0] * absC[1][0] + targetBox.AxisLen[1] * absC[1][1] + targetBox.AxisLen[2] * absC[1][2];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Src의 z축
	for (i = 0; i < 3; ++i)
	{
		c[2][i] = boundingBox.Axis[2].Dot(targetBox.Axis[i]);
		absC[2][i] = abs(c[2][i]);
		if (absC[2][i] > cutoff)
			existsParallelPair = true;
	}

	d[2] = diff.Dot(boundingBox.Axis[2]);

	r = abs(d[2]);
	r0 = boundingBox.AxisLen[2];
	r1 = targetBox.AxisLen[0] * absC[2][0] + targetBox.AxisLen[1] * absC[2][1] + targetBox.AxisLen[2] * absC[2][2];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Dest의 x축
	r = abs(diff.Dot(targetBox.Axis[0]));
	r0 = boundingBox.AxisLen[0] * absC[0][0] + boundingBox.AxisLen[1] * absC[1][0] + boundingBox.AxisLen[2] * absC[2][0];
	r1 = targetBox.AxisLen[0];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Dest의 y축
	r = abs(diff.Dot(targetBox.Axis[1]));
	r0 = boundingBox.AxisLen[0] * absC[0][1] + boundingBox.AxisLen[1] * absC[1][1] + boundingBox.AxisLen[2] * absC[2][1];
	r1 = targetBox.AxisLen[1];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Dest의 z축
	r = abs(diff.Dot(targetBox.Axis[2]));
	r0 = boundingBox.AxisLen[0] * absC[0][2] + boundingBox.AxisLen[1] * absC[1][2] + boundingBox.AxisLen[2] * absC[2][2];
	r1 = targetBox.AxisLen[2];

	if (r > r0 + r1)
	{
		return false;
	}

	if (existsParallelPair == true)
	{
		return true;
	}


	// 분리축 : Cross(Src x축, Dest x축)
	r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
	r0 = boundingBox.AxisLen[1] * absC[2][0] + boundingBox.AxisLen[2] * absC[1][0];
	r1 = targetBox.AxisLen[1] * absC[0][2] + targetBox.AxisLen[2] * absC[0][1];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src x축, Dest y축)
	r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
	r0 = boundingBox.AxisLen[1] * absC[2][1] + boundingBox.AxisLen[2] * absC[1][1];
	r1 = targetBox.AxisLen[0] * absC[0][2] + targetBox.AxisLen[2] * absC[0][0];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src x축, Dest z축)
	r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
	r0 = boundingBox.AxisLen[1] * absC[2][2] + boundingBox.AxisLen[2] * absC[1][2];
	r1 = targetBox.AxisLen[0] * absC[0][1] + targetBox.AxisLen[1] * absC[0][0];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src y축 , Dest x축)
	r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
	r0 = boundingBox.AxisLen[0] * absC[2][0] + boundingBox.AxisLen[2] * absC[0][0];
	r1 = targetBox.AxisLen[1] * absC[1][2] + targetBox.AxisLen[2] * absC[1][1];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src y축 , Dest y축)
	r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
	r0 = boundingBox.AxisLen[0] * absC[2][1] + boundingBox.AxisLen[2] * absC[0][1];
	r1 = targetBox.AxisLen[0] * absC[1][2] + targetBox.AxisLen[2] * absC[1][0];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src y축 , Dest z축)
	r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
	r0 = boundingBox.AxisLen[0] * absC[2][2] + boundingBox.AxisLen[2] * absC[0][2];
	r1 = targetBox.AxisLen[0] * absC[1][1] + targetBox.AxisLen[1] * absC[1][0];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src z축 , Dest x축)
	r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
	r0 = boundingBox.AxisLen[0] * absC[1][0] + boundingBox.AxisLen[1] * absC[0][0];
	r1 = targetBox.AxisLen[1] * absC[2][2] + targetBox.AxisLen[2] * absC[2][1];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src z축 , Dest y축)
	r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
	r0 = boundingBox.AxisLen[0] * absC[1][1] + boundingBox.AxisLen[1] * absC[0][1];
	r1 = targetBox.AxisLen[0] * absC[2][2] + targetBox.AxisLen[2] * absC[2][0];

	if (r > r0 + r1)
	{
		return false;
	}

	// 분리축 : Cross(Src z축 , Dest z축)
	r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
	r0 = boundingBox.AxisLen[0] * absC[1][2] + boundingBox.AxisLen[1] * absC[0][2];
	r1 = targetBox.AxisLen[0] * absC[2][1] + targetBox.AxisLen[1] * absC[2][0];

	if (r > r0 + r1)
	{
		return false;
	}

	return true;
}

bool CCollision::CollisionSphereToSphere(CollisionResult& SrcResult, CollisionResult& DestResult, const SphereInfo& SrcInfo, const SphereInfo& DestInfo)
{
	float DistBetween = SrcInfo.Center.Distance(DestInfo.Center);

	if (DistBetween <= SrcInfo.Radius + DestInfo.Radius)
	{
		float ExternalDiff = abs(DistBetween - (SrcInfo.Radius + DestInfo.Radius));

		// 정확하게 2개의 구가, 한개의 지점에서 충돌했을 때 
		// 즉, 구가 충돌하는 그 순간 (offset  은 0.01 정도 줄 것이다)
		// HItPoint 정보를 세팅해준다. 
		// 1) 외부의 구가 서로 충돌할 경우
		// 그리고 HitPoint 위치는, 두개의 구, Center 사이의 거리, 그 중심점이 될 것이다.
		if (ExternalDiff < 3.f)
		{
			SrcResult.HitPoint = (SrcInfo.Center + DestInfo.Center) / 2.f;
			DestResult.HitPoint = (SrcInfo.Center + DestInfo.Center) / 2.f;
		}

		// 2) 한개의 구가, 다른 구의 내부에 존재한 상태에서, 정확하게 한 점에서 충돌할 경우
		float MaxRadius = SrcInfo.Radius > DestInfo.Radius ? SrcInfo.Radius : DestInfo.Radius;
		float MinRadius = SrcInfo.Radius < DestInfo.Radius ? SrcInfo.Radius : DestInfo.Radius;

		Vector3 MaxSphereCenter = MaxRadius == SrcInfo.Radius ? SrcInfo.Center : DestInfo.Center;
		Vector3 MinSphereCenter = MinRadius == SrcInfo.Radius ? SrcInfo.Center : DestInfo.Center;
		
		float InternalDiff = abs(DistBetween - (MaxRadius - MinRadius));

		if (InternalDiff < 3.f)
		{
			// 큰 구 Center -> 작은 구 Center 방향
			Vector3 Dir = (MinSphereCenter - MaxSphereCenter);
			Dir.Normalize();

			Vector3 HitPoint = MaxSphereCenter + Dir * MaxRadius;

			SrcResult.HitPoint = HitPoint;
			DestResult.HitPoint = HitPoint;
		}

		return true;
	}

	return false;
}

bool CCollision::CollisionRayToSphere(Vector3& HitPoint, const Ray& ray, const SphereInfo& Sphere)
{
	Vector3 M = ray.Pos - Sphere.Center;

	float b = 2.f * M.Dot(ray.Dir);
	float c = M.Dot(M) - Sphere.Radius * Sphere.Radius;

	float Det = b * b - 4.f * c;

	// 교점이 없는 경우
	if (Det < 0.f)
	{
		return false;
	}

	Det = sqrtf(Det);

	float t1, t2;

	t1 = (-b + Det) / 2.f;
	t2 = (-b - Det) / 2.f;

	// 두 교점 모두 Ray 진행방향의 뒤에 있는 경우
	if (t1 < 0 && t2 < 0)
	{
		return false;
	}

	// Ray에서 가까운 교점을 기준으로 HitPoint 생성
	float Dist = t1 < t2 ? t1: t2;

	if (Dist < 0.f)
	{
		Dist = t2;
	}

	HitPoint = ray.Pos + ray.Dir * Dist;

	return true;
}

bool CCollision::CollisionRayToBox3D(float& Scale, const Ray& ray, const Box3DInfo& Box)
{

	float tMin = 0;
	float tMax = FLT_MAX;

	Scale = 1000.f;

	Vector3 Delta = Box.Center - ray.Pos;

	Vector3 n = Box.Axis[0];

	float e = n.Dot(Delta);
	float f = ray.Dir.Dot(n);

	float AABBMin = -Box.AxisLen[0];
	float AABBMax = Box.AxisLen[0];

	float t1 = (e + AABBMin) / f;
	float t2 = (e + AABBMax) / f;

	if (t1 > t2)
		std::swap(t1, t2);

	if (t2 < tMax)
		tMax = t2;

	if (t1 > tMin)
		tMin = t1;

	if (tMax < tMin)
		return false;


	n = Box.Axis[1];

	/*D1 = Box.Min.y;
	D2 = Box.Max.y;

	Denominator = n.Dot(ray.Dir);

	t1 = (-n.Dot(ray.Pos) - D1) / Denominator;
	t2 = (-n.Dot(ray.Pos) - D2) / Denominator;*/

	e = n.Dot(Delta);
	f = ray.Dir.Dot(n);

	AABBMin = -Box.AxisLen[1];
	AABBMax = Box.AxisLen[1];

	t1 = (e + AABBMin) / f;
	t2 = (e + AABBMax) / f;


	if (t1 > t2)
		std::swap(t1, t2);

	if (t2 < tMax)
		tMax = t2;

	if (t1 > tMin)
		tMin = t1;

	if (tMax < tMin)
		return false;


	n = Box.Axis[2];

	/*D1 = Box.Min.z;
	D2 = Box.Max.z;

	Denominator = n.Dot(ray.Dir);

	t1 = (-n.Dot(ray.Pos) - D1) / Denominator;
	t2 = (-n.Dot(ray.Pos) - D2) / Denominator;*/


	e = n.Dot(Delta);
	f = ray.Dir.Dot(n);

	AABBMin = -Box.AxisLen[2];
	AABBMax = Box.AxisLen[2];

	t1 = (e + AABBMin) / f;
	t2 = (e + AABBMax) / f;

	if (t1 > t2)
		std::swap(t1, t2);

	if (t2 < tMax)
		tMax = t2;

	if (t1 > tMin)
		tMin = t1;

	if (tMax < tMin)
		return false;

	/*  
	******** 여기서 tMin은 o + td 라는 Ray가 있을 때 Box와 최초로 만나게 될 때의 t값이므로 o가 Ray의 StartPos, o + td가 EndPos가 되므로 ********
	******** StartPos와 EndPos를 알게 됐으니 충돌 지점까지만 뻗어 나가게 해야 할 때 필요한 Scale을 구할 수 있다 ********
	*/
	
	Vector3 Start = ray.Pos;
	Vector3 End = (ray.Pos + ray.Dir * tMin);
	
	float Dist = Start.Distance(End);

	Scale = Dist;

	return true;
}

bool CCollision::CollisionBox3DToHalfLine(float& Scale, const Box3DInfo& Box, const HalfLineInfo& HalfLineInfo, CColliderBox3D* BoxCollider)
{
	if (Box.Min.x <= HalfLineInfo.EndPos.x && Box.Max.x >= HalfLineInfo.EndPos.x &&
		Box.Min.y <= HalfLineInfo.EndPos.y && Box.Max.y >= HalfLineInfo.EndPos.y &&
		Box.Min.z <= HalfLineInfo.EndPos.z && Box.Max.z >= HalfLineInfo.EndPos.z)
	{

		float Dist = HalfLineInfo.StartPos.Distance(HalfLineInfo.EndPos);
		Scale = Dist;

		return true;
	}

	Ray ray;
	ray.Dir = Vector3(HalfLineInfo.EndPos - HalfLineInfo.StartPos);
	ray.Dir.Normalize();
	ray.Pos = HalfLineInfo.StartPos;

	bool Intersect = CollisionRayToBox3D(Scale, ray, Box);

	if (Intersect)
	{
		// ray는 통과하지만 halfline의 endpos는 아직 통과하지 못했다면 intersect된게 아닌 코드 추가
		Vector3 Dir = HalfLineInfo.EndPos - HalfLineInfo.StartPos;

		if (Dir.x > 0.f && HalfLineInfo.EndPos.x < Box.Min.x)
			return false;
		if (Dir.x < 0.f && HalfLineInfo.EndPos.x > Box.Max.x)
			return false;

		if (Dir.y > 0.f && HalfLineInfo.EndPos.y < Box.Min.y)
			return false;
		if (Dir.y < 0.f && HalfLineInfo.EndPos.y > Box.Max.y)
			return false;

		if (Dir.z > 0.f && HalfLineInfo.EndPos.z < Box.Min.z)
			return false;
		if (Dir.z < 0.f && HalfLineInfo.EndPos.z > Box.Max.z)
			return false;

		return true;
	}

	return false;
}


bool CCollision::CollisionBox3DToSphere(Vector3& HitPoint, CColliderBox3D* Src, const Box3DInfo& Box, const SphereInfo& Sphere)
{
	// Sphere를 Box중심 좌표계로 바꾸고 Box가 회전할때 Sphere의 중심도 같이 회전해서 AABB처럼 만들어준다
	SphereInfo SphereInObbSpace;
	SphereInObbSpace.Center = Sphere.Center - Box.Center;
	SphereInObbSpace.Radius = Sphere.Radius;
	Vector3 BoxRotMat = Src->GetWorldRot();

	XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(BoxRotMat.x, BoxRotMat.y, BoxRotMat.z);

	Matrix	matRot;
	matRot.RotationQuaternion(Qut);

	SphereInObbSpace.Center = SphereInObbSpace.Center.TransformNormal(matRot);

	if (SphereInObbSpace.Center.x + SphereInObbSpace.Radius < -Box.AxisLen[0])
		return false;
	if (SphereInObbSpace.Center.x - SphereInObbSpace.Radius > Box.AxisLen[0])
		return false;
	if (SphereInObbSpace.Center.y + SphereInObbSpace.Radius < -Box.AxisLen[1])
		return false;
	if (SphereInObbSpace.Center.y - SphereInObbSpace.Radius > Box.AxisLen[1])
		return false;
	if (SphereInObbSpace.Center.z + SphereInObbSpace.Radius < -Box.AxisLen[2])
		return false;
	if (SphereInObbSpace.Center.z - SphereInObbSpace.Radius > Box.AxisLen[2])
		return false;


	//MessageBox(nullptr, TEXT("충돌"), TEXT("충돌"), MB_OK);

	return true;
}

bool CCollision::CheckPointInBox(const Vector3& Point, const Box3DInfo& Box)
{
	if (Point.x >= Box.Min.x && Point.x <= Box.Max.x &&
		Point.y >= Box.Min.y && Point.y <= Box.Max.y &&
		Point.z >= Box.Min.z && Point.z <= Box.Max.z)
		return true;

	return false;
}
