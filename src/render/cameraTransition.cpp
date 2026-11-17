//==================================================================================
// 
// カメラの遷移管理クラスのソースファイル [cameraTransition.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "render/cameraTransition.h"
#include "render/camera.h"
#include "core/manager.h"
#include "math/vec3math.h"
#include <algorithm>

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CCameraTransition *CCameraTransition::Create(CCamera *pStart,
	CCamera *pEnd,
	const int nFrame)
{
	CCameraTransition *pCameraTrans = new CCameraTransition;		// 生成したオブジェクト
	if (pCameraTrans != nullptr && FAILED(pCameraTrans->Init(pStart, pEnd, nFrame)))
	{ // 初期化失敗時、終了
		pCameraTrans->Uninit();
		return nullptr;
	}

	return pCameraTrans;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CCameraTransition::CCameraTransition() : CObject(CAMERA_TRANS_PRIORITY)
{ // タイプ指定
	SetType(TYPE::CAMERA_TRANS);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CCameraTransition::~CCameraTransition()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CCameraTransition::Init(CCamera *pStart,
	CCamera *pEnd, 
	const int nFrame)
{ // null参照防止
	if (pStart == nullptr || pEnd == nullptr) return E_POINTER;

	// 引数保存
	m_pStart = pStart;
	m_pEnd = pEnd;
	m_nFrame = nFrame;

	// 遷移終了後カメラの位置を戻すため、スタート位置のカメラの情報を保存
	m_posVStart = *pStart->GetPosV();
	m_posRStart = *pStart->GetPosR();
	m_rotStart = *pStart->GetRotate();
	m_vecUStart = *pStart->GetVecU();
	m_fNearStart = pStart->GetNear();
	m_fFarStart = pStart->GetFar();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CCameraTransition::Uninit(void)
{ // 終了
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CCameraTransition::Update(void)
{
	float t = static_cast<float>(m_nCounterFrame) / static_cast<float>(m_nFrame);		// 経過フレームの割合

	// 補間後の値を求める
	Vector3 posV = Vec3::Lerp(m_posVStart, *m_pEnd->GetPosV(), t);
	Vector3 posR = Vec3::Lerp(m_posRStart, *m_pEnd->GetPosR(), t);
	Vector3 rot = Vec3::Lerp(m_rotStart, *m_pEnd->GetRotate(), t);
	Vector3 vecU = Vec3::Lerp(m_vecUStart, *m_pEnd->GetVecU(), t);
	float fNear = std::lerp(m_fNearStart, m_pEnd->GetNear(), t);
	float fFar = std::lerp(m_fFarStart, m_pEnd->GetFar(), t);

	// 最初のカメラに適用
	m_pStart->SetPosV(posV);
	m_pStart->SetPosR(posR);
	m_pStart->SetRotate(rot);
	m_pStart->SetVecU(vecU);
	m_pStart->SetNear(fNear);
	m_pStart->SetFar(fFar);

	if (m_nCounterFrame >= m_nFrame)
	{ // フレーム数が指定フレーム数に達したなら、変更後終了
		// カメラのタイプを変更
		CCamera::SetFocus(m_pEnd->GetType());

		// 最初のカメラを元に戻す
		m_pStart->SetPosV(m_posVStart);
		m_pStart->SetPosR(m_posRStart);
		m_pStart->SetRotate(m_rotStart);
		m_pStart->SetVecU(m_vecUStart);
		m_pStart->SetNear(m_fNearStart);
		m_pStart->SetFar(m_fFarStart);
		
		Uninit();
		return;
	}

	m_nCounterFrame++;		// フレーム数増加
}