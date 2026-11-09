//==================================================================================
// 
// アイテム取得時の噛みつき演出クラスのソースファイル [itemBiteEffect.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "effect/itemBiteEffect.h"
#include "object/3d/objectBillboard3D.h"
#include "render/camera.h"
#include "render/texture.h"
#include <climits>

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float JAW_HEIGHT_RATIO = 0.5f;			// 横幅に対するあごの高さ
	constexpr float OPEN_GAP_RATIO = 0.65f;				// 横幅に対する開いた口の隙間
	constexpr float TOOTH_DEPTH_RATIO = 0.125f;			// テクスチャの牙の高さ
	constexpr float TOOTH_HALF_PITCH_RATIO = 1.0f / 32.0f;	// 16本の牙の半間隔
	constexpr float OVERLAP_MARGIN = 0.01f;					// 牙の隙間を防ぐ重なり
	constexpr int MIN_EFFECT_TIME = 3;						// 開く・閉じる・開くための最小フレーム数
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CItemBiteEffect *CItemBiteEffect::Create(const Vector3 &pos, const float fWidth, const int nTime, const Matrix *pMtxParent)
{
	CItemBiteEffect *pEffect = new CItemBiteEffect;	// 生成した演出
	if (pEffect != nullptr)
	{ // nullでなければ初期化
		pEffect->Init(pos, fWidth, nTime, pMtxParent);
	}

	return pEffect;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CItemBiteEffect::CItemBiteEffect(const int nPriority) : CObject(nPriority)
{ // タイプ指定
	SetType(TYPE::OBJ_BILLBOARD_3D);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CItemBiteEffect::~CItemBiteEffect()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CItemBiteEffect::Init(const Vector3 &pos, const float fWidth, const int nTime, const Matrix *pMtxParent)
{
	CTexture *pTexture = CTexture::GetInstance();	// テクスチャ管理

	// テクスチャ読み込み
	const UINT uTask = pTexture->Register("data/TEXTURE/fang_top.png");
	if (uTask == INVALID_TEX_ID) return S_FALSE;		// 読み込み失敗

	m_pos = pos;						// 発生位置を保存
	m_fWidth = fWidth;					// 横幅を保存
	m_nTime = nTime >= MIN_EFFECT_TIME ? nTime : MIN_EFFECT_TIME;
	m_nElapsed = 0;
	const Vector2 size(fWidth, fWidth * JAW_HEIGHT_RATIO);	// 牙のサイズ

	// ビルボードは CObject に自動登録され、各自で描画される
	m_pUpper = new CObjectBillboard3D(ITEM_BITE_EFFECT_PRIORITY);
	if (FAILED(m_pUpper->Init(pos, VECTOR3_NULL, size, INT_MAX))) return E_FAIL;
	m_pUpper->BindTexture(static_cast<int>(uTask));
	m_pUpper->SetAlpha(false);
	m_pUpper->SetParent(pMtxParent);

	m_pLower = new CObjectBillboard3D(ITEM_BITE_EFFECT_PRIORITY);
	if (FAILED(m_pLower->Init(pos, VECTOR3_NULL, size, INT_MAX))) return E_FAIL;
	m_pLower->BindTexture(static_cast<int>(uTask));
	m_pLower->SetRotation(Vector3(0.0f, D3DX_PI, 0.0f));
	m_pLower->SetAlpha(false);
	m_pLower->SetParent(pMtxParent);

	// 各ビルボードの描画前後にレンダーステートを設定
	CObjectBillboard3D *apJaw[] = { m_pUpper, m_pLower };
	for (CObjectBillboard3D *pJaw : apJaw)
	{
		pJaw->AddRenderState({ D3DRS_ZENABLE, FALSE }, false);
		pJaw->AddRenderState({ D3DRS_ZWRITEENABLE, FALSE }, false);
		pJaw->AddRenderState({ D3DRS_LIGHTING, FALSE }, false);
		pJaw->AddRenderState({ D3DRS_ZENABLE, TRUE }, true);
		pJaw->AddRenderState({ D3DRS_ZWRITEENABLE, TRUE }, true);
		pJaw->AddRenderState({ D3DRS_LIGHTING, TRUE }, true);
	}
	UpdatePose();	// 最初の描画に備えて初期位置を設定
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CItemBiteEffect::Uninit(void)
{ // 各バッファ解放
	if (m_pUpper)
	{ // 上牙解放
		m_pUpper->Uninit();
		m_pUpper = nullptr;
	}

	if (m_pLower)
	{ // 下牙解放
		m_pLower->Uninit();
		m_pLower = nullptr; 
	}

	CObject::Release();		// 管理リストからの解放を予約
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CItemBiteEffect::Update(void)
{
	if (m_bEnded) return;

	m_nElapsed++;		// 経過フレーム数を加算
	if (m_nElapsed >= m_nTime)
	{ // 演出が終われば解放
		Uninit();
		return;
	}

	UpdatePose();	// 牙の位置と透明度を更新
}

//==================================================================================
// --- 牙の位置と透明度の更新処理 ---
//==================================================================================
void CItemBiteEffect::UpdatePose(void)
{
	if (m_bEnded || !m_pUpper || !m_pLower) return;
	Vector3 up(0.0f, 1.0f, 0.0f);		// カメラがない場合の上方向
	Vector3 right(1.0f, 0.0f, 0.0f);	// カメラがない場合の右方向
	const CCamera::TYPE type = CCamera::GetFocus();	// 描画対象のカメラ

	if (CCamera::IsValid(type))
	{
		CCamera *pCamera = CCamera::GetCamera(type);
		if (pCamera)
		{ // 更新済みのカメラから画面上方向を計算
			Matrix mtxView, mtxInverse;
			D3DXMatrixLookAtLH(&mtxView, pCamera->GetPosV(), pCamera->GetPosR(), pCamera->GetVecU());
			if (D3DXMatrixInverse(&mtxInverse, nullptr, &mtxView))
			{
				up = Vector3(mtxInverse._21, mtxInverse._22, mtxInverse._23);
				right = Vector3(mtxInverse._11, mtxInverse._12, mtxInverse._13);
			}
		}
	}

	// 中央フレームで必ず閉じ、前後で開く
	const int nMiddle = (m_nTime - 1) / 2;
	const float fOpen = m_nElapsed <= nMiddle
		? 1.0f - static_cast<float>(m_nElapsed) / static_cast<float>(nMiddle)
		: static_cast<float>(m_nElapsed - nMiddle) / static_cast<float>(m_nTime - 1 - nMiddle);
	const float fAlpha = m_nElapsed <= nMiddle ? 1.0f : 1.0f - fOpen;
	const float fHeight = m_fWidth * JAW_HEIGHT_RATIO;
	const float fGap = fOpen * m_fWidth * OPEN_GAP_RATIO - (1.0f - fOpen) * fHeight * (TOOTH_DEPTH_RATIO + OVERLAP_MARGIN);
	const float fOffset = (fHeight + fGap) * 0.5f;	// 中心から上下の牙までの距離

	// 座標更新
	m_pUpper->SetPosition(m_pos + up * fOffset);
	// 下あごを半歯分ずらし、上下の凸凹を噛み合わせる
	m_pLower->SetPosition(m_pos - up * fOffset + right * (m_fWidth * TOOTH_HALF_PITCH_RATIO));

	// 色設定
	m_pUpper->SetColor(Color(1.0f, 1.0f, 1.0f, fAlpha));
	m_pLower->SetColor(Color(1.0f, 1.0f, 1.0f, fAlpha));
}
