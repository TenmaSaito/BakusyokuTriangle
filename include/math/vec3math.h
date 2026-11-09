//==================================================================================
// 
// Vector3の計算関連関数をまとめたヘッダーファイル [vec3math.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _VEC3MATH_H_
#define _VEC3MATH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** Vector3計算関連名前空間 ***
//**********************************************************************************
namespace Vec3
{
	//******************************************************************************
	// *** 3軸列挙 ***
	//******************************************************************************
	enum class Axis : char
	{
		X,		// X軸
		Y,		// Y軸
		Z,		// Z軸
		MAX
	};

	// 有効値かどうかの判定
	constexpr bool IsValid(const Axis value) noexcept
	{
		return EnumUtil::InRange(value, Axis::X, Axis::MAX);
	}

	//******************************************************************************
	// *** 軸列挙 (複数軸有り) ***
	//******************************************************************************
	enum class AxisEx : char
	{
		X = static_cast<char>(Axis::X),		// X軸
		Y = static_cast<char>(Axis::Y),		// Y軸
		Z = static_cast<char>(Axis::Z),		// Z軸
		XY,		// X + Y軸
		XZ,		// X + Z軸
		YZ,		// Y + Z軸
		XYZ,	// XYZ軸
		MAX
	};

	// 有効値かどうかの判定
	constexpr bool IsValid(const AxisEx value) noexcept
	{
		return EnumUtil::InRange(value, AxisEx::X, AxisEx::MAX);
	}

	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	float Length(const Vector3 &vec);
	float Length(const Vector3 &To, const Vector3 &From);
	float LengthSq(const Vector3 &vec);
	float LengthSq(const Vector3 &To, const Vector3 &From);
	float L2Norm(const Vector3 &vec);
	float Dot(const Vector3 &vec1, const Vector3 &vec2);
	bool IsVertical(const Vector3 &vec1, const Vector3 &vec2);
	bool IsParallel(const Vector3 &vec1, const Vector3 &vec2);
	bool IsInsideTriangle(const Vector3 &pos, const Vector3 *pVtx, const bool bInverse = false);
	bool IsInsideViewOfBeside(const Vector3 &pos, const Vector3 &posV, const Vector3 &posR, const float fovy);
	bool IsInsideViewOfVertical(const Vector3 &pos, const Vector3 &posV, const Vector3 &posR, const float fovy);
	float DistancePointToSegment(const Vector3 &point, const Vector3 &segStart, const Vector3 &segEnd);
	float Height(const Vector3 &pos, const Vector3 &vtx, const Vector3 &nor);
	Vector3 Nor(const Vector3 &origin, const Vector3 *pVtx, const bool bInverse = false);
	Vector3 Cross(const Vector3 &vec1, const Vector3 &vec2);
	Vector3 Normalize(const Vector3 &vec);
	Vector3 Lerp(const Vector3 &start, const Vector3 &end, const float t);
	Vector3 Middle(const Vector3 &p1, const Vector3 &p2);
	Vector3 Direction(const Vector3 &To, const Vector3 &From);
	Vector3 Direction(const Vector3 &angle);
	Vector3 Random(void);
	Vector3 Random(const Vector3 &min, const Vector3 &max);
	Vector3 Clamp(const Vector3 &vec, const Vector3 &min, const Vector3 &max);
	Vector3 FixedRotation(const Vector3 &rot);
	Vector3 Arc(const float fRadius, const float fTheta, const float fPhi, const Vector3 &offset = VECTOR3_NULL);
	Vector3 &Fill(Vector3 &rVec, const float fValue);
	Vector3 Fill(const float fValue);
	Vector3 ToRadian(const Vector3 &degree, const bool bFixed = false);
	Vector3 ToDegree(const Vector3 &radian, const bool bFixed = false);
	Vector2 ToVector2(const Vector3 &vec, const Axis less = Axis::Z);
	void AssignAxis(Vector3 &vec, const Axis axis, const float fValue);
	float GetAxis(Vector3 &vec, const Axis axis);
	AxisEx ToAxisEx(const Axis axis);
	Axis ToAxis(const AxisEx axisEx);
}
#endif