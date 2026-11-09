//==================================================================================
// 
// 暗転フェードの遷移クラスのソースファイル [fadeTransition.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/transition/fadeTransition.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "sound/sound.h"
#include "render/texture.h"
#include "math/color.h"
#include "math/rand.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
constexpr ConstVector3 FIRST_POSITION = ConstVector3(0.0f, SCREEN_MIDDLE.y, 0.0f);		// ラインの座標サイズ
constexpr ConstVector2 FIRST_LINESIZE = ConstVector2(1.0f, 15.0f);		// ラインの初期サイズ
constexpr ConstVector2 FIRST_OUTLINESIZE = ConstVector2(1.0f, 25.0f);		// アウトラインの初期サイズ
constexpr float CABLE_SCALE_UP_VALUE = 70.0f;			// 電線のサイズの拡大値
constexpr float VOLT_SCALE_UP_VALUE_X = 35.0f;			// 電流のサイズの拡大値_X
constexpr float VOLT_SCALE_UP_VALUE_Y = 1.15f;			// 電流のサイズの拡大倍率_Y
constexpr float SCALE_DOWN_VALUE = 0.85f;			// サイズの縮小倍率
constexpr float ORBIT_POS_Y_RANGE = 125.0f;		// オービットの移動幅
constexpr float MIN_ORBIT_WIDTH = 10.0f;			// オービットの最小サイズ
constexpr float MAX_ORBIT_WIDTH = 25.0f;			// オービットの最大サイズ

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CFadeTransition::CFadeTransition()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CFadeTransition::~CFadeTransition()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CFadeTransition::Init(void)
{ // ポリゴン生成
	CManager *pManager = CManager::GetInstance();	// マネージャー
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイス

	// 頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * DEFAULT_VERTEX_NUM,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点をロック
	m_pVtxBuff->Lock(0, 0, reinterpret_cast<void**>(&pVtx), 0);

	// 座標設定
	pVtx[0].pos = Vector3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = Vector3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx[2].pos = Vector3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// rhw設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 色設定
	pVtx[0].col = Color(0.0f, 0.0f, 0.0f, 0.0f);
	pVtx[1].col = Color(0.0f, 0.0f, 0.0f, 0.0f);
	pVtx[2].col = Color(0.0f, 0.0f, 0.0f, 0.0f);
	pVtx[3].col = Color(0.0f, 0.0f, 0.0f, 0.0f);

	// uv設定
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点をアンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CFadeTransition::Uninit(void)
{ // バッファ破棄
	SafeRelease(m_pVtxBuff);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CFadeTransition::Update(void)
{
	STATE state = GetState();		// 遷移状態

	if (state == STATE::WIPE_IN)
	{ // 遷移演出中(ワイプイン)なら
		m_nTransitionTime--;		// 時間減少
		UpdateTransitionIn();
	}
	else if (state == STATE::WIPE_OUT)
	{ // 遷移演出中(ワイプアウト)なら
		UpdateTransitionOut();
	}

	// 頂点座標の更新
	UpdateVertex();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CFadeTransition::Draw(void)
{ // ポリゴンの描画 (待機状態の場合、描画スキップ)
	if (GetState() == STATE::STAY) return;

	CManager *pManager = CManager::GetInstance();	// マネージャー
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイス
	
	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// テクスチャ設定
	pDevice->SetTexture(0, nullptr);

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- 遷移演出開始処理 --
//==================================================================================
void CFadeTransition::StartTransition(const int nTransitionTime, const CScene::MODE modeNext)
{ // ワイプイン開始
	SetState(STATE::WIPE_IN);
	SetModeNext(modeNext);

	// 引数の保存
	m_nTransitionTime = nTransitionTime;
	m_fAlpha = 0.0f;
	m_fAlphaAdd = 1.0f / nTransitionTime;
}

//==================================================================================
// --- 遷移演出終了処理 ---
//==================================================================================
void CFadeTransition::EndTransition(const int nTransitionTime)
{ // ワイプアウト開始
	SetState(STATE::WIPE_OUT);
}

//==================================================================================
// --- 遷移演出中(ワイプイン)処理 ---
//==================================================================================
void CFadeTransition::UpdateTransitionIn(void)
{
	m_fAlpha += m_fAlphaAdd;		// α値加算

	// 頂点情報更新
	UpdateVertex();

	if (m_fAlpha >= 1.0f)
	{ // α値が1.0fになったら、画面が隠れるため遷移
		m_fAlphaAdd *= -1;			// α値を下げるため、符号逆転
		SetState(STATE::WIPE_OUT);
		CManager::GetInstance()->SetMode(GetNextMode());
		return;
	}
}

//==================================================================================
// --- 遷移演出中(ワイプアウト)処理 ---
//==================================================================================
void CFadeTransition::UpdateTransitionOut(void)
{
	m_fAlpha += m_fAlphaAdd;		// α値加算

	// 頂点情報更新
	UpdateVertex();

	if (m_fAlpha <= 0.0f)
	{ // α値が0.0fになったら、画面が全て現れるため、終了
		m_fAlphaAdd *= -1;			// α値を上げるため、符号逆転
		SetState(STATE::STAY);
	}
}

//==================================================================================
// --- 頂点座標更新処理 ---
//==================================================================================
void CFadeTransition::UpdateVertex(void)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点をロック
	m_pVtxBuff->Lock(0, 0, reinterpret_cast<void **>(&pVtx), 0);

	// 色設定
	pVtx[0].col = Color(0.0f, 0.0f, 0.0f, m_fAlpha);
	pVtx[1].col = Color(0.0f, 0.0f, 0.0f, m_fAlpha);
	pVtx[2].col = Color(0.0f, 0.0f, 0.0f, m_fAlpha);
	pVtx[3].col = Color(0.0f, 0.0f, 0.0f, m_fAlpha);

	// 頂点をアンロック
	m_pVtxBuff->Unlock();
}