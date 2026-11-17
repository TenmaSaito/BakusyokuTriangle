//==================================================================================
// 
// カメラの遷移管理クラスのヘッダーファイル [item.h]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "render/cameraShot.h"
#include "render/cameraTransition.h"
#include "util/util.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CCameraShot *CCameraShot::Create(CCamera *pStart,
	CCamera *pEnd,
	const int nTransFrameOutbound,
	const int nTransFrameInbound,
	const int nWaitFrame)
{
	return Util::Create<CCameraShot>(pStart, 
		pEnd, 
		nTransFrameOutbound,
		nTransFrameInbound,
		nWaitFrame);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CCameraShot::CCameraShot() : CObject(FIRST_PRIORITY)
{ // タイプ指定
	SetType(TYPE::CAMERA_SHOT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CCameraShot::~CCameraShot()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CCameraShot::Init(CCamera *pStart,
	CCamera *pEnd,
	const int nTransFrameOutbound,
	const int nTransFrameInbound,
	const int nWaitFrame)
{ // 引数を保存
	m_pStart = pStart;
	m_pEnd = pEnd;
	m_nTransFrameOutbound = nTransFrameOutbound;
	m_nTransFrameInbound = nTransFrameInbound;
	m_nWaitFrame = nWaitFrame;

	// nullptr参照を防止
	if (pStart == nullptr || pEnd == nullptr) return E_POINTER;

	// カメラの往路遷移
	if (CCameraTransition::Create(m_pStart, m_pEnd, m_nTransFrameOutbound) == nullptr)
	{ // 生成失敗
		return E_FAIL;
	}

	// 成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CCameraShot::Uninit(void)
{ // 終了
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CCameraShot::Update(void)
{
	if (m_nCountFrame >= m_nTransFrameOutbound + m_nWaitFrame)
	{ // フレームカウントが往路 + 待機フレーム分経過した場合、復路遷移開始 + 終了
		CCameraTransition::Create(m_pEnd, m_pStart, m_nTransFrameInbound);
		Uninit();
		return;
	}

	m_nCountFrame++;		// フレームカウント増加
}