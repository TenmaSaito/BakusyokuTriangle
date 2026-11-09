//==================================================================================
//
// 牙演出付きポーズクラスのソースファイル [bitePause.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "ui/bitePause.h"
#include "object/2d/polygon2D.h"
#include "render/texture.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view JAW_TEXTURE = "data/TEXTURE/fang_top.png";		// 牙のテクスチャ
	constexpr int BITE_FRAME = 12;				// 牙が閉じるまでのフレーム数
	constexpr float JAW_HEIGHT = 36.0f;			// 牙の高さ
	constexpr float JAW_WIDTH_RATIO = 1.1f;		// 選択中の項目の幅に対する牙の幅
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CBitePause *CBitePause::Create(void)
{
	CBitePause *pPause = new CBitePause;		// 生成したポーズへのポインタ
	if (FAILED(pPause->Init()))
	{ // 生成に失敗した場合、破棄
		pPause->Uninit();
		delete pPause;
		return nullptr;
	}

	return pPause;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CBitePause::CBitePause()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CBitePause::~CBitePause()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CBitePause::Init(void)
{ // 親クラスの初期化処理
	HRESULT hr = CPause::Init();
	if (FAILED(hr)) return hr;

	// テクスチャ読み込み
	const UINT uTexture = CTexture::GetInstance()->Register(JAW_TEXTURE);
	if (uTexture == INVALID_TEX_ID) return E_FAIL;		// 読み込み失敗

	// 牙のポリゴンを生成 (大きさは UpdateJaw で選択中の項目に合わせる)
	m_pUpper.reset(CPolygon2D::Create(VECTOR3_NULL, VECTOR3_NULL, Vector2(JAW_HEIGHT, JAW_HEIGHT)));
	m_pLower.reset(CPolygon2D::Create(VECTOR3_NULL, VECTOR3_NULL, Vector2(JAW_HEIGHT, JAW_HEIGHT)));
	if (m_pUpper == nullptr || m_pLower == nullptr) return E_FAIL;

	// テクスチャを紐づけ
	m_pUpper->BindTexture(static_cast<int>(uTexture));
	m_pLower->BindTexture(static_cast<int>(uTexture));

	// 下の牙は同じ画像を半回転して使用
	m_pLower->SetRotation(Vector3(0.0f, D3DX_PI, 0.0f));

	UpdateJaw();		// 最初の描画に備えて位置を設定

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CBitePause::Uninit(void)
{
	if (m_pUpper != nullptr)
	{ // 上の牙を破棄
		m_pUpper->Uninit();
		m_pUpper.reset();
	}

	if (m_pLower != nullptr)
	{ // 下の牙を破棄
		m_pLower->Uninit();
		m_pLower.reset();
	}

	// 親クラスの終了処理
	CPause::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CBitePause::Update(void)
{
	if (GetDisp() != true)
	{ // ポーズを閉じたら次回用にリセット
		m_bBiting = false;
		m_nBiteFrame = 0;
		return;
	}

	if (m_bBiting == true)
	{ // 牙を閉じる間は選択入力を受け付けない
		m_nBiteFrame++;
		UpdateJaw();

		if (m_nBiteFrame >= BITE_FRAME)
		{ // 閉じ切ったら決定処理を実行 (遷移中に開き直さないよう、フレーム数は閉じたまま残す)
			m_bBiting = false;
			ExecuteType(m_selectedType);
		}

		return;
	}

	// 親クラスの更新処理 (決定時は ActionType が呼ばれる)
	CPause::Update();

	UpdateJaw();		// 選択中の項目に牙を合わせる
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CBitePause::Draw(void)
{ // 親クラスの描画処理
	CPause::Draw();

	// 描画フラグが降りている場合、スキップ
	if (GetDisp() != true) return;

	if (m_pUpper != nullptr) m_pUpper->Draw();
	if (m_pLower != nullptr) m_pLower->Draw();
}

//==================================================================================
// --- タイプ別処理 (決定時に噛みつきを開始する) ---
//==================================================================================
void CBitePause::ActionType(void)
{ // 決定したタイプを保存し、噛みつき開始
	m_selectedType = GetType();
	m_nBiteFrame = 0;
	m_bBiting = true;
}

//==================================================================================
// --- 牙の位置更新処理 ---
//==================================================================================
void CBitePause::UpdateJaw(void)
{
	if (m_pUpper == nullptr || m_pLower == nullptr) return;

	// 噛みつき中は決定した項目、それ以外は選択中の項目を挟む
	const TYPE type = (m_bBiting == true) ? m_selectedType : GetType();
	const CPolygon2D *pMenu = GetMenu(type);		// 挟む項目のポリゴン
	if (pMenu == nullptr) return;

	const Vector3 &posMenu = *pMenu->GetPosition();		// 項目の中心
	const Vector2 &sizeMenu = *pMenu->GetSize();		// 項目の大きさ
	const float fProgress = static_cast<float>(m_nBiteFrame) / static_cast<float>(BITE_FRAME);		// 閉じ具合 (0 ～ 1)
	const float fOffset = (sizeMenu.y * (1.0f - fProgress) + JAW_HEIGHT) * 0.5f;		// 項目の中心から牙までの距離
	const Vector2 sizeJaw(sizeMenu.x * JAW_WIDTH_RATIO, JAW_HEIGHT);		// 牙の大きさ

	// 大きさと位置を設定
	m_pUpper->SetSize(sizeJaw);
	m_pLower->SetSize(sizeJaw);
	m_pUpper->SetPosition(Vector3(posMenu.x, posMenu.y - fOffset, 0.0f));
	m_pLower->SetPosition(Vector3(posMenu.x, posMenu.y + fOffset, 0.0f));
}
