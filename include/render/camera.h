//==================================================================================
// 
// カメラクラスのヘッダーファイル [camera.h]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
#ifndef _CAMERA_H_		// インクルードガード
#define _CAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr float DEFAULT_FOVY = 45.0f;		// 視野角
inline constexpr D3DVIEWPORT9 DEFAULT_VP = D3DVIEWPORT9{ 0,0,SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f };		// デフォルトのビューポート

//**********************************************************************************
// *** カメラクラス ***
//**********************************************************************************
class CCamera
{
public:
	// カメラの種類
	enum class TYPE
	{
		FREE = 0,		// フリーカメラ
		PLAYER,		// プレイヤーカメラ
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const TYPE value) noexcept
	{
		return EnumUtil::InRange(value, TYPE::FREE, TYPE::MAX);
	}

	CCamera(const TYPE type);
	virtual ~CCamera();

	static CCamera *Create(const Vector3 &posV, const Vector3 &posR);

	virtual void Init(const Vector3 &posV, const Vector3 &posR);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void SetCamera(void);
	bool IsVisible(const Vector3 &pos);
	void SetPosV(const Vector3 &posV) { m_posV = posV; }
	const Vector3 *GetPosV(void) const { return &m_posV; }
	void SetPosR(const Vector3 &posR) { m_posR = posR; }
	const Vector3 *GetPosR(void) const { return &m_posR; }
	void SetVecU(const Vector3 &vecU) { m_vecU = vecU; }
	const Vector3 *GetVecU(void) const { return &m_vecU; }
	void SetRotate(const Vector3 &rotate) { m_rot = rotate; }
	const Vector3 *GetRotate(void) const { return &m_rot; }
	void SetFovy(const float fFovy) { m_fFovy = fFovy; }
	float GetFovy(void) const {	return m_fFovy; }
	void SetNear(const float fNear) { m_fNear = fNear; }
	float GetNear(void) const { return m_fNear; }
	void SetFar(const float fFar) { m_fFar = fFar; }
	float GetFar(void) const { return m_fFar; }
	Vector3 GetRay(void) const;
	Vector3 GetRayToScreen(const Vector3 &screenPos);
	void SetViewport(const D3DVIEWPORT9 &vp) { m_vp = vp; }
	const D3DVIEWPORT9 *GetViewport(void) const { return &m_vp; }
	void SetFocus(void) { m_focusType = m_type; }
	void SetPixelFog(Color col, float fStart, float fEnd);
	void SetEnablePixelFog(const bool bEnable) { m_bEnableFog = bEnable; }

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static TYPE GetTarget(void);
	static void SetFocus(const TYPE type) { m_focusType = type; }
	static TYPE GetFocus(void) { return m_focusType; }
	static CCamera *Begin(const TYPE type);
	static void End(void);
	static CCamera *GetCamera(const TYPE type) { return IsValid(type) ? m_apCamera[EnumUtil::ToIndex(type)] : nullptr; }

protected:
	void Release(void);

private:
	inline static CCamera *m_apCamera[EnumUtil::ToIndex(TYPE::MAX)] = {};			// 現在登録されているカメラへのポインタ
	inline static D3DVIEWPORT9 m_vpDef = DEFAULT_VP;			// デフォルトのビューポート
	inline static TYPE m_currentType = CCamera::TYPE::FREE;		// 現在設置されているカメラのタイプ
	inline static TYPE m_focusType = CCamera::TYPE::MAX;			// フルスクリーンにフォーカスしているカメラのタイプ
	Matrix m_mtxProjection;		// プロジェクションマトリックス
	Matrix m_mtxView;		// ビューマトリックス
	Vector3 m_posV;			// 視点	
	Vector3 m_posR;			// 注視点
	Vector3 m_vecU;			// 上方向ベクトル
	Vector3 m_rot;			// 向き
	D3DVIEWPORT9 m_vp;		// ビューポート設定
	Color m_colFog;			// ピクセルフォグの色
	float m_fStart, m_fEnd;	// フォグの開始・終了位置
	float m_fFovy;			// 視野角
	float m_fNear, m_fFar;	// カメラの描画最小・最長距離
	TYPE m_type;			// カメラの種類
	bool m_bEnableOrtho;	// 正射影投影の有効化
	bool m_bEnableFog;		// フォグの有効化
};
#endif