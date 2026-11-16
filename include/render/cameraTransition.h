//==================================================================================
// 
// カメラの遷移管理クラスのヘッダーファイル [cameraTransition.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _CAMERA_TRANSITION_H_
#define _CAMERA_TRANSITION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CCamera;

//**********************************************************************************
// *** カメラの遷移管理クラス ***
//**********************************************************************************
class CCameraTransition : public CObject
{
public:
	CCameraTransition();
	~CCameraTransition();

	static CCameraTransition *Create(CCamera *pStart,
		CCamera *pEnd,
		const int nFrame);

	HRESULT Init(CCamera *pStart,
		CCamera *pEnd, 
		const int nFrame);
	void Uninit(void);
	void Update(void);

private:
	static constexpr int CAMERA_TRANS_PRIORITY = FIRST_PRIORITY;		// 遷移の優先順位
	CCamera *m_pStart = nullptr;		// 開始カメラ
	CCamera *m_pEnd = nullptr;			// 目標カメラ
	Vector3 m_posVStart = VECTOR3_NULL;	// 開始視点位置
	Vector3 m_posRStart = VECTOR3_NULL;	// 開始注視点位置
	Vector3 m_rotStart = VECTOR3_NULL;	// 開始角度
	Vector3 m_vecUStart = VECTOR3_NULL;	// 開始上方向ベクトル
	float m_fNearStart = 0.0f;		// 開始描画最近距離
	float m_fFarStart = 0.0f;		// 開始描画最遠距離
	int m_nFrame = 0.0f;			// 遷移時間
	int m_nCounterFrame = 0.0f;		// 経過フレーム
};
#endif