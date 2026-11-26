//==================================================================================
//
// constexprで定義できる行列・クォータニオンの型のヘッダーファイル [constMatrix.h]
// Author : TENMA SAITO
//
// D3DXMATRIX / D3DXQUATERNION はコンストラクタがconstexprではないため、
// 定数はこのファイルの型で定義し、使用時にD3DXの型へ暗黙変換する。
//
// foundation : システムのヘッダー以外に依存しない (pchからインクルードしてよい)
//
//==================================================================================
#ifndef _CONST_MATRIX_H_		// インクルードガード
#define _CONST_MATRIX_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <d3dx9.h>
#include <type_traits>

//**********************************************************************************
// *** constexpr対応のクォータニオン ***
//**********************************************************************************
struct ConstQuaternion
{
	float x = 0.0f;		// X成分
	float y = 0.0f;		// Y成分
	float z = 0.0f;		// Z成分
	float w = 1.0f;		// W成分 (既定は回転なし)

	constexpr ConstQuaternion() noexcept = default;
	constexpr ConstQuaternion(const float fx, const float fy, const float fz, const float fw) noexcept : x(fx), y(fy), z(fz), w(fw) {}

	// D3DXの型へ変換
	operator D3DXQUATERNION() const { return D3DXQUATERNION(x, y, z, w); }

	// D3DXのポインタとして渡す (D3DX関数の引数用)
	D3DXQUATERNION *AsD3DX(void) noexcept { return reinterpret_cast<D3DXQUATERNION *>(this); }
	const D3DXQUATERNION *AsD3DX(void) const noexcept { return reinterpret_cast<const D3DXQUATERNION *>(this); }
};

//**********************************************************************************
// *** constexpr対応の4x4行列 ***
//**********************************************************************************
struct ConstMatrix
{
	float _11 = 1.0f, _12 = 0.0f, _13 = 0.0f, _14 = 0.0f;	// 1行目
	float _21 = 0.0f, _22 = 1.0f, _23 = 0.0f, _24 = 0.0f;	// 2行目
	float _31 = 0.0f, _32 = 0.0f, _33 = 1.0f, _34 = 0.0f;	// 3行目
	float _41 = 0.0f, _42 = 0.0f, _43 = 0.0f, _44 = 1.0f;	// 4行目 (既定は単位行列)

	constexpr ConstMatrix() noexcept = default;
	constexpr ConstMatrix(
		const float f11, const float f12, const float f13, const float f14,
		const float f21, const float f22, const float f23, const float f24,
		const float f31, const float f32, const float f33, const float f34,
		const float f41, const float f42, const float f43, const float f44) noexcept
		: _11(f11), _12(f12), _13(f13), _14(f14)
		, _21(f21), _22(f22), _23(f23), _24(f24)
		, _31(f31), _32(f32), _33(f33), _34(f34)
		, _41(f41), _42(f42), _43(f43), _44(f44) {}

	// D3DXの型へ変換
	operator D3DXMATRIX() const
	{
		return D3DXMATRIX(
			_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44);
	}

	// D3DXのポインタとして渡す (D3DX関数の引数用)
	D3DXMATRIX *AsD3DX(void) noexcept { return reinterpret_cast<D3DXMATRIX *>(this); }
	const D3DXMATRIX *AsD3DX(void) const noexcept { return reinterpret_cast<const D3DXMATRIX *>(this); }
};

//**********************************************************************************
// *** D3DXの型とのメモリ配置の一致確認 (AsD3DXの前提) ***
//**********************************************************************************
static_assert(sizeof(ConstQuaternion) == sizeof(D3DXQUATERNION) && std::is_standard_layout_v<ConstQuaternion>, "ConstQuaternionのメモリ配置がD3DXQUATERNIONと一致しません");
static_assert(sizeof(ConstMatrix) == sizeof(D3DXMATRIX) && std::is_standard_layout_v<ConstMatrix>, "ConstMatrixのメモリ配置がD3DXMATRIXと一致しません");
#endif
