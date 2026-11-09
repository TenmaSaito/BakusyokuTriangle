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
// *** インクルードファイル ***
//**********************************************************************************
#include <compare>
#include <cmath>
#include <limits>
#include <type_traits>

//**********************************************************************************
// *** ConstVector内部で使う補助処理 ***
//**********************************************************************************
namespace ConstVectorDetail
{
	inline constexpr float DEFAULT_EPSILON = 1.0e-5f;		// 近似比較の既定の許容誤差
	inline constexpr float ANGLE_EPSILON = 1.0e-15f;		// 角度計算で長さ0とみなす値
	inline constexpr float MIN_SMOOTH_TIME = 1.0e-4f;		// SmoothDampの最小時間
	inline constexpr float RAD_TO_DEG = 180.0f / 3.14159265f;	// ラジアンから度への変換値
	inline constexpr float FLOAT_INF = std::numeric_limits<float>::infinity();	// 無限大

	//==================================================================================
	// --- 平方根の計算処理 (コンパイル時はニュートン法、実行時はstd::sqrt) ---
	//==================================================================================
	constexpr float Sqrt(const float f) noexcept
	{
		if (!std::is_constant_evaluated()) return std::sqrt(f);
		if (!(f > 0.0f)) return f;

		// 初期値を平方根以上にすると値は単調に減るので、減らなくなった時点で収束
		const double dValue = static_cast<double>(f);
		double dCurrent = (dValue > 1.0) ? dValue : 1.0;
		while (true)
		{
			const double dNext = 0.5 * (dCurrent + dValue / dCurrent);
			if (!(dNext < dCurrent)) break;
			dCurrent = dNext;
		}
		return static_cast<float>(dCurrent);
	}

	//==================================================================================
	// --- 絶対値の取得処理 ---
	//==================================================================================
	constexpr float Abs(const float f) noexcept
	{
		return (f < 0.0f) ? -f : f;
	}

	//==================================================================================
	// --- 最小値の取得処理 ---
	//==================================================================================
	constexpr float Min(const float a, const float b) noexcept
	{
		return (a < b) ? a : b;
	}

	//==================================================================================
	// --- 最大値の取得処理 ---
	//==================================================================================
	constexpr float Max(const float a, const float b) noexcept
	{
		return (a > b) ? a : b;
	}

	//==================================================================================
	// --- 範囲制限処理 ---
	//==================================================================================
	constexpr float Clamp(const float f, const float fMin, const float fMax) noexcept
	{
		return (f < fMin) ? fMin : ((f > fMax) ? fMax : f);
	}

	//==================================================================================
	// --- 0～1への範囲制限処理 ---
	//==================================================================================
	constexpr float Clamp01(const float f) noexcept
	{
		return Clamp(f, 0.0f, 1.0f);
	}

	//==================================================================================
	// --- なす角の計算処理 (内積と長さの2乗の積から度数で求める) ---
	//==================================================================================
	inline float AngleFromDot(const float fDot, const float fSqrMagnitudeProduct) noexcept
	{
		if (fSqrMagnitudeProduct < ANGLE_EPSILON) return 0.0f;
		return std::acos(Clamp(fDot / std::sqrt(fSqrMagnitudeProduct), -1.0f, 1.0f)) * RAD_TO_DEG;
	}

	//==================================================================================
	// --- 目標へ滑らかに近づける処理 (2次元・3次元共通) ---
	//==================================================================================
	template <class Vec>
	constexpr Vec SmoothDamp(const Vec &current, const Vec &target, Vec &velocity, const float fSmoothTime, const float fDeltaTime, const float fMaxSpeed) noexcept
	{
		const float fTime = Max(fSmoothTime, MIN_SMOOTH_TIME);
		const float fOmega = 2.0f / fTime;
		const float fX = fOmega * fDeltaTime;
		const float fDecay = 1.0f / (1.0f + fX + 0.48f * fX * fX + 0.235f * fX * fX * fX);

		const Vec change = Vec::ClampMagnitude(current - target, fMaxSpeed * fTime);
		const Vec temp = (velocity + change * fOmega) * fDeltaTime;
		const Vec output = (current - change) + (change + temp) * fDecay;

		velocity = (velocity - temp * fOmega) * fDecay;

		// 目標を通り過ぎたら目標で止める
		if (Vec::Dot(target - current, output - target) > 0.0f)
		{
			velocity = Vec();
			return target;
		}
		return output;
	}
}

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
struct ConstVector3;

//**********************************************************************************
// *** constexpr対応の2次元ベクトル ***
//**********************************************************************************
struct ConstVector2
{
	float x = 0.0f;		// X成分
	float y = 0.0f;		// Y成分

	// 定数 (クラス内では型が未完成なのでconstで宣言し、クラス外でconstexpr定義する)
	static const ConstVector2 Zero;				// (0, 0)
	static const ConstVector2 One;				// (1, 1)
	static const ConstVector2 Up;				// (0, 1)
	static const ConstVector2 Down;				// (0, -1)
	static const ConstVector2 Left;				// (-1, 0)
	static const ConstVector2 Right;			// (1, 0)
	static const ConstVector2 PositiveInfinity;	// (+∞, +∞)
	static const ConstVector2 NegativeInfinity;	// (-∞, -∞)

	constexpr ConstVector2() noexcept = default;
	constexpr ConstVector2(const float fx, const float fy) noexcept : x(fx), y(fy) {}

	// D3DXの型へ変換
	operator D3DXVECTOR2() const;
	D3DXVECTOR2 *AsD3DX(void) noexcept;				// D3DX関数の出力引数へ渡す用
	const D3DXVECTOR2 *AsD3DX(void) const noexcept;	// D3DX関数の入力引数へ渡す用

	// 演算子
	constexpr ConstVector2 operator+(const ConstVector2 &v) const noexcept;
	constexpr ConstVector2 operator-(const ConstVector2 &v) const noexcept;
	constexpr ConstVector2 operator*(const float f) const noexcept;
	constexpr ConstVector2 operator/(const float f) const noexcept;
	constexpr ConstVector2 operator-() const noexcept;

	// 比較演算子 (x, y の順で比較。== も同時に生成される)
	constexpr auto operator<=>(const ConstVector2 &) const noexcept = default;

	// 複合代入
	constexpr ConstVector2 &operator+=(const ConstVector2 &v) noexcept;
	constexpr ConstVector2 &operator-=(const ConstVector2 &v) noexcept;
	constexpr ConstVector2 &operator*=(const float f) noexcept;
	constexpr ConstVector2 &operator/=(const float f) noexcept;

	// ユーティリティ
	constexpr float Dot(const ConstVector2 &v) const noexcept;
	constexpr float Cross(const ConstVector2 &v) const noexcept;			// 外積のZ成分
	constexpr float Magnitude(void) const noexcept;
	constexpr float SqrMagnitude(void) const noexcept;
	constexpr float Distance(const ConstVector2 &v) const noexcept;
	constexpr float SqrDistance(const ConstVector2 &v) const noexcept;
	constexpr ConstVector2 Normalized(void) const noexcept;					// 長さ0なら0ベクトル
	constexpr ConstVector2 &Normalize(void) noexcept;						// 自身を正規化
	constexpr ConstVector2 &Set(const float fx, const float fy) noexcept;
	constexpr ConstVector2 &Scale(const ConstVector2 &v) noexcept;			// 自身を成分ごとに乗算
	constexpr ConstVector2 Lerp(const ConstVector2 &to, const float fT) const noexcept;
	constexpr ConstVector2 Clamp(const ConstVector2 &vMin, const ConstVector2 &vMax) const noexcept;
	constexpr bool NearlyEqual(const ConstVector2 &v, const float fEpsilon = ConstVectorDetail::DEFAULT_EPSILON) const noexcept;
	constexpr bool IsZero(const float fEpsilon = ConstVectorDetail::DEFAULT_EPSILON) const noexcept;
	constexpr ConstVector2 WithX(const float fX) const noexcept;
	constexpr ConstVector2 WithY(const float fY) const noexcept;
	constexpr ConstVector3 ToVector3(const float fZ = 0.0f) const noexcept;

	// staticユーティリティ
	static constexpr float Dot(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr float Cross(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr float Magnitude(const ConstVector2 &v) noexcept;
	static constexpr float SqrMagnitude(const ConstVector2 &v) noexcept;
	static constexpr float Distance(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr float SqrDistance(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr ConstVector2 Normalize(const ConstVector2 &v) noexcept;
	static constexpr ConstVector2 Lerp(const ConstVector2 &a, const ConstVector2 &b, const float fT) noexcept;				// fTは0～1に制限
	static constexpr ConstVector2 LerpUnclamped(const ConstVector2 &a, const ConstVector2 &b, const float fT) noexcept;
	static constexpr ConstVector2 Min(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr ConstVector2 Max(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr ConstVector2 Clamp(const ConstVector2 &v, const ConstVector2 &vMin, const ConstVector2 &vMax) noexcept;
	static constexpr ConstVector2 Scale(const ConstVector2 &a, const ConstVector2 &b) noexcept;
	static constexpr ConstVector2 MoveTowards(const ConstVector2 &current, const ConstVector2 &target, const float fMaxDelta) noexcept;
	static constexpr ConstVector2 ClampMagnitude(const ConstVector2 &v, const float fMaxLength) noexcept;
	static constexpr ConstVector2 Reflect(const ConstVector2 &direction, const ConstVector2 &normal) noexcept;
	static constexpr ConstVector2 Perpendicular(const ConstVector2 &v) noexcept;											// 反時計回りに90度回転
	static constexpr ConstVector2 SmoothDamp(const ConstVector2 &current, const ConstVector2 &target, ConstVector2 &velocity, const float fSmoothTime, const float fDeltaTime, const float fMaxSpeed = ConstVectorDetail::FLOAT_INF) noexcept;
	static constexpr bool NearlyEqual(const ConstVector2 &a, const ConstVector2 &b, const float fEpsilon = ConstVectorDetail::DEFAULT_EPSILON) noexcept;
	static float Angle(const ConstVector2 &from, const ConstVector2 &to) noexcept;											// 0～180度
	static float SignedAngle(const ConstVector2 &from, const ConstVector2 &to) noexcept;									// -180～180度
};

//**********************************************************************************
// *** constexpr対応の3次元ベクトル ***
//**********************************************************************************
struct ConstVector3
{
	float x = 0.0f;		// X成分
	float y = 0.0f;		// Y成分
	float z = 0.0f;		// Z成分

	// 定数 (クラス内では型が未完成なのでconstで宣言し、クラス外でconstexpr定義する)
	static const ConstVector3 Zero;				// (0, 0, 0)
	static const ConstVector3 One;				// (1, 1, 1)
	static const ConstVector3 Up;				// (0, 1, 0)
	static const ConstVector3 Down;				// (0, -1, 0)
	static const ConstVector3 Left;				// (-1, 0, 0)
	static const ConstVector3 Right;			// (1, 0, 0)
	static const ConstVector3 Forward;			// (0, 0, 1)
	static const ConstVector3 Back;				// (0, 0, -1)
	static const ConstVector3 PositiveInfinity;	// (+∞, +∞, +∞)
	static const ConstVector3 NegativeInfinity;	// (-∞, -∞, -∞)

	constexpr ConstVector3() noexcept = default;
	constexpr ConstVector3(const float fx, const float fy, const float fz) noexcept : x(fx), y(fy), z(fz) {}

	// D3DXの型へ変換
	operator D3DXVECTOR3() const;
	D3DXVECTOR3 *AsD3DX(void) noexcept;				// D3DX関数の出力引数へ渡す用
	const D3DXVECTOR3 *AsD3DX(void) const noexcept;	// D3DX関数の入力引数へ渡す用

	// 演算子
	constexpr ConstVector3 operator+(const ConstVector3 &v) const noexcept;
	constexpr ConstVector3 operator-(const ConstVector3 &v) const noexcept;
	constexpr ConstVector3 operator*(const float f) const noexcept;
	constexpr ConstVector3 operator/(const float f) const noexcept;
	constexpr ConstVector3 operator-() const noexcept;

	// 比較演算子 (x, y, z の順で比較。== も同時に生成される)
	constexpr auto operator<=>(const ConstVector3 &) const noexcept = default;

	// 複合代入
	constexpr ConstVector3 &operator+=(const ConstVector3 &v) noexcept;
	constexpr ConstVector3 &operator-=(const ConstVector3 &v) noexcept;
	constexpr ConstVector3 &operator*=(const float f) noexcept;
	constexpr ConstVector3 &operator/=(const float f) noexcept;

	// ユーティリティ
	constexpr float Dot(const ConstVector3 &v) const noexcept;
	constexpr ConstVector3 Cross(const ConstVector3 &v) const noexcept;
	constexpr float Magnitude(void) const noexcept;
	constexpr float SqrMagnitude(void) const noexcept;
	constexpr float Distance(const ConstVector3 &v) const noexcept;
	constexpr float SqrDistance(const ConstVector3 &v) const noexcept;
	constexpr ConstVector3 Normalized(void) const noexcept;					// 長さ0なら0ベクトル
	constexpr ConstVector3 &Normalize(void) noexcept;						// 自身を正規化
	constexpr ConstVector3 &Set(const float fx, const float fy, const float fz) noexcept;
	constexpr ConstVector3 &Scale(const ConstVector3 &v) noexcept;			// 自身を成分ごとに乗算
	constexpr ConstVector3 Lerp(const ConstVector3 &to, const float fT) const noexcept;
	constexpr ConstVector3 Clamp(const ConstVector3 &vMin, const ConstVector3 &vMax) const noexcept;
	constexpr bool NearlyEqual(const ConstVector3 &v, const float fEpsilon = ConstVectorDetail::DEFAULT_EPSILON) const noexcept;
	constexpr bool IsZero(const float fEpsilon = ConstVectorDetail::DEFAULT_EPSILON) const noexcept;
	constexpr ConstVector3 WithX(const float fX) const noexcept;
	constexpr ConstVector3 WithY(const float fY) const noexcept;
	constexpr ConstVector3 WithZ(const float fZ) const noexcept;
	constexpr ConstVector2 ToVector2(void) const noexcept;					// Zを捨てる

	// staticユーティリティ
	static constexpr float Dot(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr ConstVector3 Cross(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr float Magnitude(const ConstVector3 &v) noexcept;
	static constexpr float SqrMagnitude(const ConstVector3 &v) noexcept;
	static constexpr float Distance(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr float SqrDistance(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr ConstVector3 Normalize(const ConstVector3 &v) noexcept;
	static constexpr ConstVector3 Lerp(const ConstVector3 &a, const ConstVector3 &b, const float fT) noexcept;				// fTは0～1に制限
	static constexpr ConstVector3 LerpUnclamped(const ConstVector3 &a, const ConstVector3 &b, const float fT) noexcept;
	static constexpr ConstVector3 Min(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr ConstVector3 Max(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr ConstVector3 Clamp(const ConstVector3 &v, const ConstVector3 &vMin, const ConstVector3 &vMax) noexcept;
	static constexpr ConstVector3 Scale(const ConstVector3 &a, const ConstVector3 &b) noexcept;
	static constexpr ConstVector3 MoveTowards(const ConstVector3 &current, const ConstVector3 &target, const float fMaxDelta) noexcept;
	static constexpr ConstVector3 ClampMagnitude(const ConstVector3 &v, const float fMaxLength) noexcept;
	static constexpr ConstVector3 Reflect(const ConstVector3 &direction, const ConstVector3 &normal) noexcept;
	static constexpr ConstVector3 Project(const ConstVector3 &v, const ConstVector3 &onNormal) noexcept;
	static constexpr ConstVector3 ProjectOnPlane(const ConstVector3 &v, const ConstVector3 &planeNormal) noexcept;
	static constexpr ConstVector3 SmoothDamp(const ConstVector3 &current, const ConstVector3 &target, ConstVector3 &velocity, const float fSmoothTime, const float fDeltaTime, const float fMaxSpeed = ConstVectorDetail::FLOAT_INF) noexcept;
	static constexpr bool NearlyEqual(const ConstVector3 &a, const ConstVector3 &b, const float fEpsilon = ConstVectorDetail::DEFAULT_EPSILON) noexcept;
	static float Angle(const ConstVector3 &from, const ConstVector3 &to) noexcept;											// 0～180度
	static float SignedAngle(const ConstVector3 &from, const ConstVector3 &to, const ConstVector3 &axis) noexcept;			// -180～180度
};

//**********************************************************************************
// *** ConstVector2の実装 ***
//**********************************************************************************

//==================================================================================
// --- 定数の定義 ---
//==================================================================================
inline constexpr ConstVector2 ConstVector2::Zero = ConstVector2(0.0f, 0.0f);
inline constexpr ConstVector2 ConstVector2::One = ConstVector2(1.0f, 1.0f);
inline constexpr ConstVector2 ConstVector2::Up = ConstVector2(0.0f, 1.0f);
inline constexpr ConstVector2 ConstVector2::Down = ConstVector2(0.0f, -1.0f);
inline constexpr ConstVector2 ConstVector2::Left = ConstVector2(-1.0f, 0.0f);
inline constexpr ConstVector2 ConstVector2::Right = ConstVector2(1.0f, 0.0f);
inline constexpr ConstVector2 ConstVector2::PositiveInfinity = ConstVector2(ConstVectorDetail::FLOAT_INF, ConstVectorDetail::FLOAT_INF);
inline constexpr ConstVector2 ConstVector2::NegativeInfinity = ConstVector2(-ConstVectorDetail::FLOAT_INF, -ConstVectorDetail::FLOAT_INF);

//==================================================================================
// --- D3DXVECTOR2への変換処理 ---
//==================================================================================
inline ConstVector2::operator D3DXVECTOR2() const
{
	return D3DXVECTOR2(x, y);
}

//==================================================================================
// --- D3DXVECTOR2のポインタへの変換処理 (D3DX関数の出力引数へ渡す用) ---
//==================================================================================
inline D3DXVECTOR2 *ConstVector2::AsD3DX(void) noexcept
{
	return reinterpret_cast<D3DXVECTOR2 *>(this);
}

//==================================================================================
// --- D3DXVECTOR2のポインタへの変換処理 (D3DX関数の入力引数へ渡す用) ---
//==================================================================================
inline const D3DXVECTOR2 *ConstVector2::AsD3DX(void) const noexcept
{
	return reinterpret_cast<const D3DXVECTOR2 *>(this);
}

//==================================================================================
// --- D3DXVECTOR2とのメモリ配置の一致確認 ---
//==================================================================================
static_assert(sizeof(ConstVector2) == sizeof(D3DXVECTOR2) && std::is_standard_layout_v<ConstVector2>, "ConstVector2のメモリ配置がD3DXVECTOR2と一致しません");

//==================================================================================
// --- 演算子のオーバーロード (加算) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::operator+(const ConstVector2 &v) const noexcept
{
	return { x + v.x, y + v.y };
}

//==================================================================================
// --- 演算子のオーバーロード (減算) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::operator-(const ConstVector2 &v) const noexcept
{
	return { x - v.x, y - v.y };
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー乗算) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::operator*(const float f) const noexcept
{
	return { x * f, y * f };
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー除算) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::operator/(const float f) const noexcept
{
	return { x / f, y / f };
}

//==================================================================================
// --- 演算子のオーバーロード (符号反転) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::operator-() const noexcept
{
	return { -x, -y };
}

//==================================================================================
// --- 演算子のオーバーロード (スカラーが左辺の乗算) ---
//==================================================================================
constexpr ConstVector2 operator*(const float f, const ConstVector2 &v) noexcept
{
	return v * f;
}

//==================================================================================
// --- 演算子のオーバーロード (加算代入) ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::operator+=(const ConstVector2 &v) noexcept
{
	x += v.x;
	y += v.y;
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (減算代入) ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::operator-=(const ConstVector2 &v) noexcept
{
	x -= v.x;
	y -= v.y;
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー乗算代入) ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::operator*=(const float f) noexcept
{
	x *= f;
	y *= f;
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー除算代入) ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::operator/=(const float f) noexcept
{
	x /= f;
	y /= f;
	return *this;
}

//==================================================================================
// --- 内積の計算処理 ---
//==================================================================================
constexpr float ConstVector2::Dot(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return a.x * b.x + a.y * b.y;
}

//==================================================================================
// --- 外積のZ成分の計算処理 ---
//==================================================================================
constexpr float ConstVector2::Cross(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return a.x * b.y - a.y * b.x;
}

//==================================================================================
// --- 長さの2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector2::SqrMagnitude(const ConstVector2 &v) noexcept
{
	return Dot(v, v);
}

//==================================================================================
// --- 長さの計算処理 ---
//==================================================================================
constexpr float ConstVector2::Magnitude(const ConstVector2 &v) noexcept
{
	return ConstVectorDetail::Sqrt(SqrMagnitude(v));
}

//==================================================================================
// --- 2点間の距離の2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector2::SqrDistance(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return SqrMagnitude(b - a);
}

//==================================================================================
// --- 2点間の距離の計算処理 ---
//==================================================================================
constexpr float ConstVector2::Distance(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return Magnitude(b - a);
}

//==================================================================================
// --- 正規化したベクトルの取得処理 (長さ0なら0ベクトル) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Normalize(const ConstVector2 &v) noexcept
{
	const float fLength = Magnitude(v);
	return (fLength > 0.0f) ? v / fLength : ConstVector2();
}

//==================================================================================
// --- 線形補間処理 (fTの範囲制限なし) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::LerpUnclamped(const ConstVector2 &a, const ConstVector2 &b, const float fT) noexcept
{
	return a + (b - a) * fT;
}

//==================================================================================
// --- 線形補間処理 (fTは0～1に制限) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Lerp(const ConstVector2 &a, const ConstVector2 &b, const float fT) noexcept
{
	return LerpUnclamped(a, b, ConstVectorDetail::Clamp01(fT));
}

//==================================================================================
// --- 成分ごとの最小値の取得処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Min(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return { ConstVectorDetail::Min(a.x, b.x), ConstVectorDetail::Min(a.y, b.y) };
}

//==================================================================================
// --- 成分ごとの最大値の取得処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Max(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return { ConstVectorDetail::Max(a.x, b.x), ConstVectorDetail::Max(a.y, b.y) };
}

//==================================================================================
// --- 成分ごとの範囲制限処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Clamp(const ConstVector2 &v, const ConstVector2 &vMin, const ConstVector2 &vMax) noexcept
{
	return { ConstVectorDetail::Clamp(v.x, vMin.x, vMax.x), ConstVectorDetail::Clamp(v.y, vMin.y, vMax.y) };
}

//==================================================================================
// --- 成分ごとの乗算処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Scale(const ConstVector2 &a, const ConstVector2 &b) noexcept
{
	return { a.x * b.x, a.y * b.y };
}

//==================================================================================
// --- 目標へ最大fMaxDeltaだけ近づける処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::MoveTowards(const ConstVector2 &current, const ConstVector2 &target, const float fMaxDelta) noexcept
{
	const ConstVector2 toTarget = target - current;
	const float fSqrLength = SqrMagnitude(toTarget);
	if (fSqrLength == 0.0f || (fMaxDelta >= 0.0f && fSqrLength <= fMaxDelta * fMaxDelta)) return target;
	return current + toTarget / ConstVectorDetail::Sqrt(fSqrLength) * fMaxDelta;
}

//==================================================================================
// --- 長さの上限制限処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::ClampMagnitude(const ConstVector2 &v, const float fMaxLength) noexcept
{
	return (SqrMagnitude(v) > fMaxLength * fMaxLength) ? Normalize(v) * fMaxLength : v;
}

//==================================================================================
// --- 反射ベクトルの計算処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Reflect(const ConstVector2 &direction, const ConstVector2 &normal) noexcept
{
	return direction - normal * (2.0f * Dot(direction, normal));
}

//==================================================================================
// --- 垂直なベクトルの取得処理 (反時計回りに90度回転) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Perpendicular(const ConstVector2 &v) noexcept
{
	return { -v.y, v.x };
}

//==================================================================================
// --- 目標へ滑らかに近づける処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::SmoothDamp(const ConstVector2 &current, const ConstVector2 &target, ConstVector2 &velocity, const float fSmoothTime, const float fDeltaTime, const float fMaxSpeed) noexcept
{
	return ConstVectorDetail::SmoothDamp(current, target, velocity, fSmoothTime, fDeltaTime, fMaxSpeed);
}

//==================================================================================
// --- 近似比較処理 ---
//==================================================================================
constexpr bool ConstVector2::NearlyEqual(const ConstVector2 &a, const ConstVector2 &b, const float fEpsilon) noexcept
{
	return ConstVectorDetail::Abs(a.x - b.x) <= fEpsilon && ConstVectorDetail::Abs(a.y - b.y) <= fEpsilon;
}

//==================================================================================
// --- なす角の計算処理 (0～180度) ---
//==================================================================================
inline float ConstVector2::Angle(const ConstVector2 &from, const ConstVector2 &to) noexcept
{
	return ConstVectorDetail::AngleFromDot(Dot(from, to), SqrMagnitude(from) * SqrMagnitude(to));
}

//==================================================================================
// --- 符号付きのなす角の計算処理 (-180～180度) ---
//==================================================================================
inline float ConstVector2::SignedAngle(const ConstVector2 &from, const ConstVector2 &to) noexcept
{
	return (Cross(from, to) >= 0.0f) ? Angle(from, to) : -Angle(from, to);
}

//==================================================================================
// --- 指定ベクトルとの内積の計算処理 ---
//==================================================================================
constexpr float ConstVector2::Dot(const ConstVector2 &v) const noexcept
{
	return Dot(*this, v);
}

//==================================================================================
// --- 指定ベクトルとの外積のZ成分の計算処理 ---
//==================================================================================
constexpr float ConstVector2::Cross(const ConstVector2 &v) const noexcept
{
	return Cross(*this, v);
}

//==================================================================================
// --- 自身の長さの計算処理 ---
//==================================================================================
constexpr float ConstVector2::Magnitude(void) const noexcept
{
	return Magnitude(*this);
}

//==================================================================================
// --- 自身の長さの2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector2::SqrMagnitude(void) const noexcept
{
	return SqrMagnitude(*this);
}

//==================================================================================
// --- 指定ベクトルとの距離の計算処理 ---
//==================================================================================
constexpr float ConstVector2::Distance(const ConstVector2 &v) const noexcept
{
	return Distance(*this, v);
}

//==================================================================================
// --- 指定ベクトルとの距離の2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector2::SqrDistance(const ConstVector2 &v) const noexcept
{
	return SqrDistance(*this, v);
}

//==================================================================================
// --- 正規化したベクトルの取得処理 (自身は変更しない) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Normalized(void) const noexcept
{
	return Normalize(*this);
}

//==================================================================================
// --- 自身の正規化処理 ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::Normalize(void) noexcept
{
	*this = Normalize(*this);
	return *this;
}

//==================================================================================
// --- 成分の設定処理 ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::Set(const float fx, const float fy) noexcept
{
	x = fx;
	y = fy;
	return *this;
}

//==================================================================================
// --- 自身を成分ごとに乗算する処理 ---
//==================================================================================
constexpr ConstVector2 &ConstVector2::Scale(const ConstVector2 &v) noexcept
{
	*this = Scale(*this, v);
	return *this;
}

//==================================================================================
// --- 指定ベクトルへの線形補間処理 (fTは0～1に制限) ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Lerp(const ConstVector2 &to, const float fT) const noexcept
{
	return Lerp(*this, to, fT);
}

//==================================================================================
// --- 成分ごとの範囲制限処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::Clamp(const ConstVector2 &vMin, const ConstVector2 &vMax) const noexcept
{
	return Clamp(*this, vMin, vMax);
}

//==================================================================================
// --- 指定ベクトルとの近似比較処理 ---
//==================================================================================
constexpr bool ConstVector2::NearlyEqual(const ConstVector2 &v, const float fEpsilon) const noexcept
{
	return NearlyEqual(*this, v, fEpsilon);
}

//==================================================================================
// --- 0ベクトルかどうかの近似判定処理 ---
//==================================================================================
constexpr bool ConstVector2::IsZero(const float fEpsilon) const noexcept
{
	return NearlyEqual(*this, ConstVector2(), fEpsilon);
}

//==================================================================================
// --- X成分だけを変えたベクトルの取得処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::WithX(const float fX) const noexcept
{
	return { fX, y };
}

//==================================================================================
// --- Y成分だけを変えたベクトルの取得処理 ---
//==================================================================================
constexpr ConstVector2 ConstVector2::WithY(const float fY) const noexcept
{
	return { x, fY };
}

//**********************************************************************************
// *** ConstVector3の実装 ***
//**********************************************************************************

//==================================================================================
// --- 定数の定義 ---
//==================================================================================
inline constexpr ConstVector3 ConstVector3::Zero = ConstVector3(0.0f, 0.0f, 0.0f);
inline constexpr ConstVector3 ConstVector3::One = ConstVector3(1.0f, 1.0f, 1.0f);
inline constexpr ConstVector3 ConstVector3::Up = ConstVector3(0.0f, 1.0f, 0.0f);
inline constexpr ConstVector3 ConstVector3::Down = ConstVector3(0.0f, -1.0f, 0.0f);
inline constexpr ConstVector3 ConstVector3::Left = ConstVector3(-1.0f, 0.0f, 0.0f);
inline constexpr ConstVector3 ConstVector3::Right = ConstVector3(1.0f, 0.0f, 0.0f);
inline constexpr ConstVector3 ConstVector3::Forward = ConstVector3(0.0f, 0.0f, 1.0f);
inline constexpr ConstVector3 ConstVector3::Back = ConstVector3(0.0f, 0.0f, -1.0f);
inline constexpr ConstVector3 ConstVector3::PositiveInfinity = ConstVector3(ConstVectorDetail::FLOAT_INF, ConstVectorDetail::FLOAT_INF, ConstVectorDetail::FLOAT_INF);
inline constexpr ConstVector3 ConstVector3::NegativeInfinity = ConstVector3(-ConstVectorDetail::FLOAT_INF, -ConstVectorDetail::FLOAT_INF, -ConstVectorDetail::FLOAT_INF);

//==================================================================================
// --- D3DXVECTOR3への変換処理 ---
//==================================================================================
inline ConstVector3::operator D3DXVECTOR3() const
{
	return D3DXVECTOR3(x, y, z);
}

//==================================================================================
// --- D3DXVECTOR3のポインタへの変換処理 (D3DX関数の出力引数へ渡す用) ---
//==================================================================================
inline D3DXVECTOR3 *ConstVector3::AsD3DX(void) noexcept
{
	return reinterpret_cast<D3DXVECTOR3 *>(this);
}

//==================================================================================
// --- D3DXVECTOR3のポインタへの変換処理 (D3DX関数の入力引数へ渡す用) ---
//==================================================================================
inline const D3DXVECTOR3 *ConstVector3::AsD3DX(void) const noexcept
{
	return reinterpret_cast<const D3DXVECTOR3 *>(this);
}

//==================================================================================
// --- D3DXVECTOR3とのメモリ配置の一致確認 ---
//==================================================================================
static_assert(sizeof(ConstVector3) == sizeof(D3DXVECTOR3) && std::is_standard_layout_v<ConstVector3>, "ConstVector3のメモリ配置がD3DXVECTOR3と一致しません");

//==================================================================================
// --- 演算子のオーバーロード (加算) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::operator+(const ConstVector3 &v) const noexcept
{
	return { x + v.x, y + v.y, z + v.z };
}

//==================================================================================
// --- 演算子のオーバーロード (減算) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::operator-(const ConstVector3 &v) const noexcept
{
	return { x - v.x, y - v.y, z - v.z };
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー乗算) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::operator*(const float f) const noexcept
{
	return { x * f, y * f, z * f };
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー除算) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::operator/(const float f) const noexcept
{
	return { x / f, y / f, z / f };
}

//==================================================================================
// --- 演算子のオーバーロード (符号反転) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::operator-() const noexcept
{
	return { -x, -y, -z };
}

//==================================================================================
// --- 演算子のオーバーロード (スカラーが左辺の乗算) ---
//==================================================================================
constexpr ConstVector3 operator*(const float f, const ConstVector3 &v) noexcept
{
	return v * f;
}

//==================================================================================
// --- 演算子のオーバーロード (加算代入) ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::operator+=(const ConstVector3 &v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (減算代入) ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::operator-=(const ConstVector3 &v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー乗算代入) ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::operator*=(const float f) noexcept
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (スカラー除算代入) ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::operator/=(const float f) noexcept
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

//==================================================================================
// --- 内積の計算処理 ---
//==================================================================================
constexpr float ConstVector3::Dot(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//==================================================================================
// --- 外積の計算処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Cross(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

//==================================================================================
// --- 長さの2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector3::SqrMagnitude(const ConstVector3 &v) noexcept
{
	return Dot(v, v);
}

//==================================================================================
// --- 長さの計算処理 ---
//==================================================================================
constexpr float ConstVector3::Magnitude(const ConstVector3 &v) noexcept
{
	return ConstVectorDetail::Sqrt(SqrMagnitude(v));
}

//==================================================================================
// --- 2点間の距離の2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector3::SqrDistance(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return SqrMagnitude(b - a);
}

//==================================================================================
// --- 2点間の距離の計算処理 ---
//==================================================================================
constexpr float ConstVector3::Distance(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return Magnitude(b - a);
}

//==================================================================================
// --- 正規化したベクトルの取得処理 (長さ0なら0ベクトル) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Normalize(const ConstVector3 &v) noexcept
{
	const float fLength = Magnitude(v);
	return (fLength > 0.0f) ? v / fLength : ConstVector3();
}

//==================================================================================
// --- 線形補間処理 (fTの範囲制限なし) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::LerpUnclamped(const ConstVector3 &a, const ConstVector3 &b, const float fT) noexcept
{
	return a + (b - a) * fT;
}

//==================================================================================
// --- 線形補間処理 (fTは0～1に制限) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Lerp(const ConstVector3 &a, const ConstVector3 &b, const float fT) noexcept
{
	return LerpUnclamped(a, b, ConstVectorDetail::Clamp01(fT));
}

//==================================================================================
// --- 成分ごとの最小値の取得処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Min(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return { ConstVectorDetail::Min(a.x, b.x), ConstVectorDetail::Min(a.y, b.y), ConstVectorDetail::Min(a.z, b.z) };
}

//==================================================================================
// --- 成分ごとの最大値の取得処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Max(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return { ConstVectorDetail::Max(a.x, b.x), ConstVectorDetail::Max(a.y, b.y), ConstVectorDetail::Max(a.z, b.z) };
}

//==================================================================================
// --- 成分ごとの範囲制限処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Clamp(const ConstVector3 &v, const ConstVector3 &vMin, const ConstVector3 &vMax) noexcept
{
	return { ConstVectorDetail::Clamp(v.x, vMin.x, vMax.x), ConstVectorDetail::Clamp(v.y, vMin.y, vMax.y), ConstVectorDetail::Clamp(v.z, vMin.z, vMax.z) };
}

//==================================================================================
// --- 成分ごとの乗算処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Scale(const ConstVector3 &a, const ConstVector3 &b) noexcept
{
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}

//==================================================================================
// --- 目標へ最大fMaxDeltaだけ近づける処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::MoveTowards(const ConstVector3 &current, const ConstVector3 &target, const float fMaxDelta) noexcept
{
	const ConstVector3 toTarget = target - current;
	const float fSqrLength = SqrMagnitude(toTarget);
	if (fSqrLength == 0.0f || (fMaxDelta >= 0.0f && fSqrLength <= fMaxDelta * fMaxDelta)) return target;
	return current + toTarget / ConstVectorDetail::Sqrt(fSqrLength) * fMaxDelta;
}

//==================================================================================
// --- 長さの上限制限処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::ClampMagnitude(const ConstVector3 &v, const float fMaxLength) noexcept
{
	return (SqrMagnitude(v) > fMaxLength * fMaxLength) ? Normalize(v) * fMaxLength : v;
}

//==================================================================================
// --- 反射ベクトルの計算処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Reflect(const ConstVector3 &direction, const ConstVector3 &normal) noexcept
{
	return direction - normal * (2.0f * Dot(direction, normal));
}

//==================================================================================
// --- ベクトルへの投影処理 (長さ0のベクトルへは0ベクトル) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Project(const ConstVector3 &v, const ConstVector3 &onNormal) noexcept
{
	const float fSqrLength = SqrMagnitude(onNormal);
	return (fSqrLength > 0.0f) ? onNormal * (Dot(v, onNormal) / fSqrLength) : ConstVector3();
}

//==================================================================================
// --- 平面への投影処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::ProjectOnPlane(const ConstVector3 &v, const ConstVector3 &planeNormal) noexcept
{
	return v - Project(v, planeNormal);
}

//==================================================================================
// --- 目標へ滑らかに近づける処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::SmoothDamp(const ConstVector3 &current, const ConstVector3 &target, ConstVector3 &velocity, const float fSmoothTime, const float fDeltaTime, const float fMaxSpeed) noexcept
{
	return ConstVectorDetail::SmoothDamp(current, target, velocity, fSmoothTime, fDeltaTime, fMaxSpeed);
}

//==================================================================================
// --- 近似比較処理 ---
//==================================================================================
constexpr bool ConstVector3::NearlyEqual(const ConstVector3 &a, const ConstVector3 &b, const float fEpsilon) noexcept
{
	return ConstVectorDetail::Abs(a.x - b.x) <= fEpsilon && ConstVectorDetail::Abs(a.y - b.y) <= fEpsilon && ConstVectorDetail::Abs(a.z - b.z) <= fEpsilon;
}

//==================================================================================
// --- なす角の計算処理 (0～180度) ---
//==================================================================================
inline float ConstVector3::Angle(const ConstVector3 &from, const ConstVector3 &to) noexcept
{
	return ConstVectorDetail::AngleFromDot(Dot(from, to), SqrMagnitude(from) * SqrMagnitude(to));
}

//==================================================================================
// --- 符号付きのなす角の計算処理 (-180～180度) ---
//==================================================================================
inline float ConstVector3::SignedAngle(const ConstVector3 &from, const ConstVector3 &to, const ConstVector3 &axis) noexcept
{
	return (Dot(axis, Cross(from, to)) >= 0.0f) ? Angle(from, to) : -Angle(from, to);
}

//==================================================================================
// --- 指定ベクトルとの内積の計算処理 ---
//==================================================================================
constexpr float ConstVector3::Dot(const ConstVector3 &v) const noexcept
{
	return Dot(*this, v);
}

//==================================================================================
// --- 指定ベクトルとの外積の計算処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Cross(const ConstVector3 &v) const noexcept
{
	return Cross(*this, v);
}

//==================================================================================
// --- 自身の長さの計算処理 ---
//==================================================================================
constexpr float ConstVector3::Magnitude(void) const noexcept
{
	return Magnitude(*this);
}

//==================================================================================
// --- 自身の長さの2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector3::SqrMagnitude(void) const noexcept
{
	return SqrMagnitude(*this);
}

//==================================================================================
// --- 指定ベクトルとの距離の計算処理 ---
//==================================================================================
constexpr float ConstVector3::Distance(const ConstVector3 &v) const noexcept
{
	return Distance(*this, v);
}

//==================================================================================
// --- 指定ベクトルとの距離の2乗の計算処理 ---
//==================================================================================
constexpr float ConstVector3::SqrDistance(const ConstVector3 &v) const noexcept
{
	return SqrDistance(*this, v);
}

//==================================================================================
// --- 正規化したベクトルの取得処理 (自身は変更しない) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Normalized(void) const noexcept
{
	return Normalize(*this);
}

//==================================================================================
// --- 自身の正規化処理 ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::Normalize(void) noexcept
{
	*this = Normalize(*this);
	return *this;
}

//==================================================================================
// --- 成分の設定処理 ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::Set(const float fx, const float fy, const float fz) noexcept
{
	x = fx;
	y = fy;
	z = fz;
	return *this;
}

//==================================================================================
// --- 自身を成分ごとに乗算する処理 ---
//==================================================================================
constexpr ConstVector3 &ConstVector3::Scale(const ConstVector3 &v) noexcept
{
	*this = Scale(*this, v);
	return *this;
}

//==================================================================================
// --- 指定ベクトルへの線形補間処理 (fTは0～1に制限) ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Lerp(const ConstVector3 &to, const float fT) const noexcept
{
	return Lerp(*this, to, fT);
}

//==================================================================================
// --- 成分ごとの範囲制限処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::Clamp(const ConstVector3 &vMin, const ConstVector3 &vMax) const noexcept
{
	return Clamp(*this, vMin, vMax);
}

//==================================================================================
// --- 指定ベクトルとの近似比較処理 ---
//==================================================================================
constexpr bool ConstVector3::NearlyEqual(const ConstVector3 &v, const float fEpsilon) const noexcept
{
	return NearlyEqual(*this, v, fEpsilon);
}

//==================================================================================
// --- 0ベクトルかどうかの近似判定処理 ---
//==================================================================================
constexpr bool ConstVector3::IsZero(const float fEpsilon) const noexcept
{
	return NearlyEqual(*this, ConstVector3(), fEpsilon);
}

//==================================================================================
// --- X成分だけを変えたベクトルの取得処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::WithX(const float fX) const noexcept
{
	return { fX, y, z };
}

//==================================================================================
// --- Y成分だけを変えたベクトルの取得処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::WithY(const float fY) const noexcept
{
	return { x, fY, z };
}

//==================================================================================
// --- Z成分だけを変えたベクトルの取得処理 ---
//==================================================================================
constexpr ConstVector3 ConstVector3::WithZ(const float fZ) const noexcept
{
	return { x, y, fZ };
}

//==================================================================================
// --- 2次元ベクトルへの変換処理 (Zを捨てる) ---
//==================================================================================
constexpr ConstVector2 ConstVector3::ToVector2(void) const noexcept
{
	return { x, y };
}

//==================================================================================
// --- 3次元ベクトルへの変換処理 (ConstVector3の定義後でないと書けないためここで定義) ---
//==================================================================================
constexpr ConstVector3 ConstVector2::ToVector3(const float fZ) const noexcept
{
	return { x, y, fZ };
}

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