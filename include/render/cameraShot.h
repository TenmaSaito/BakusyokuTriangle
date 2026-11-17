//==================================================================================
// 
// カメラの遷移管理クラスのヘッダーファイル [item.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _CAMERA_SHOT_H_
#define _CAMERA_SHOT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CCamera;

//**********************************************************************************
// *** カメラの一時遷移管理クラス ***
//**********************************************************************************
class CCameraShot : public CObject
{
public:
	CCameraShot();
	~CCameraShot();

	static CCameraShot *Create(CCamera *pStart,
		CCamera *pEnd,
		const int nTransFrameOutbound,
		const int nTransFrameInbound,
		const int nWaitFrame);

	HRESULT Init(CCamera *pStart,
		CCamera *pEnd,
		const int nTransFrameOutbound,
		const int nTransFrameInbound,
		const int nWaitFrame);
	void Uninit(void);
	void Update(void);

private:
	static constexpr int CAMERA_SHOT_PRIORITY = FIRST_PRIORITY;		// 遷移管理の優先順位
	int m_nTransFrameOutbound = 0;		// 往路にかかるフレーム
	int m_nTransFrameInbound = 0;		// 復路にかかるフレーム
	int m_nWaitFrame = 0;			// 待機フレーム
	int m_nCountFrame = 0;			// フレームカウント
	CCamera *m_pStart = nullptr;	// スタートするカメラ
	CCamera *m_pEnd = nullptr;		// 折り返し地点のカメラ
};
#endif