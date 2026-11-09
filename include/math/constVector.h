//==================================================================================
//
// constexprで定義できるベクトル・色の型のヘッダーファイル [constVector.h]
// Author : TENMA SAITO
//
// D3DXVECTOR2 / D3DXVECTOR3 / D3DXCOLOR はコンストラクタがconstexprではないため、
// 定数はこのファイルの型で定義し、使用時にD3DXの型へ暗黙変換する。
//
//==================================================================================
#ifndef _CONST_VECTOR_H_		// インクルードガード
#define _CONST_VECTOR_H_

//**********************************************************************************
// *** constexpr対応の2次元ベクトル ***
//**********************************************************************************
struct ConstVector2
{
	float x = 0.0f;		// X成分
	float y = 0.0f;		// Y成分

	constexpr ConstVector2() noexcept = default;
	constexpr ConstVector2(const float fx, const float fy) noexcept : x(fx), y(fy) {}

	// D3DXの型へ変換
	operator D3DXVECTOR2() const { return D3DXVECTOR2(x, y); }

	// 演算子
	constexpr ConstVector2 operator+(const ConstVector2 &v) const noexcept { return { x + v.x, y + v.y }; }
	constexpr ConstVector2 operator-(const ConstVector2 &v) const noexcept { return { x - v.x, y - v.y }; }
	constexpr ConstVector2 operator*(const float f) const noexcept { return { x * f, y * f }; }
	constexpr ConstVector2 operator/(const float f) const noexcept { return { x / f, y / f }; }
	constexpr ConstVector2 operator-() const noexcept { return { -x, -y }; }
};
constexpr ConstVector2 operator*(const float f, const ConstVector2 &v) noexcept { return v * f; }

//**********************************************************************************
// *** constexpr対応の3次元ベクトル ***
//**********************************************************************************
struct ConstVector3
{
	float x = 0.0f;		// X成分
	float y = 0.0f;		// Y成分
	float z = 0.0f;		// Z成分

	constexpr ConstVector3() noexcept = default;
	constexpr ConstVector3(const float fx, const float fy, const float fz) noexcept : x(fx), y(fy), z(fz) {}

	// D3DXの型へ変換
	operator D3DXVECTOR3() const { return D3DXVECTOR3(x, y, z); }

	// 演算子
	constexpr ConstVector3 operator+(const ConstVector3 &v) const noexcept { return { x + v.x, y + v.y, z + v.z }; }
	constexpr ConstVector3 operator-(const ConstVector3 &v) const noexcept { return { x - v.x, y - v.y, z - v.z }; }
	constexpr ConstVector3 operator*(const float f) const noexcept { return { x * f, y * f, z * f }; }
	constexpr ConstVector3 operator/(const float f) const noexcept { return { x / f, y / f, z / f }; }
	constexpr ConstVector3 operator-() const noexcept { return { -x, -y, -z }; }
};
constexpr ConstVector3 operator*(const float f, const ConstVector3 &v) noexcept { return v * f; }

//**********************************************************************************
// *** constexpr対応の色 ***
//**********************************************************************************
struct ConstColor
{
	float r = 0.0f;		// 赤
	float g = 0.0f;		// 緑
	float b = 0.0f;		// 青
	float a = 0.0f;		// α

	constexpr ConstColor() noexcept = default;
	constexpr ConstColor(const float fr, const float fg, const float fb, const float fa) noexcept : r(fr), g(fg), b(fb), a(fa) {}

	// D3DXの型へ変換
	operator D3DXCOLOR() const { return D3DXCOLOR(r, g, b, a); }

	// α値だけを変えた色を返す
	constexpr ConstColor WithAlpha(const float fAlpha) const noexcept { return { r, g, b, fAlpha }; }
};
#endif