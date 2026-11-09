//==================================================================================
// 
// 画面下に表示されるUIクラスのソースファイル [nextInfo.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "ui/nextInfo.h"
#include "object/2d/object2D.h"
#include "render/texture.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view NORMAL_BASE = "data/TEXTURE/selfs/next.png";		// 通常のBaseテクスチャ
	constexpr std::string_view ANGRY_BASE = "data/TEXTURE/selfs/angry.png";		// 通常のAngryテクスチャ
	constexpr int WAIT_ALPHA_DECREASE_FRAME = 30;		// α値減少待機フレーム
	constexpr float MIN_ALPHA = 0.3f;		// α値の最小値
	constexpr float BASE_POS_Y_RATE = 0.85f;		// ベースUIのY座標 (画面の高さに対する割合)
	constexpr float BASE_WIDTH_RATE = 0.9f;		// ベースUIの幅 (画面の幅に対する割合)
	constexpr float BASE_HEIGHT = 200.0f;			// ベースUIの高さ
	constexpr float ALPHA_DECREASE = 0.01f;		// 1フレームあたりのα値の減少量
	constexpr float NEXT_POS_X_RATE = 0.94f;		// 次のアイテムのX座標 (画面中央に対する割合)
	constexpr float NEXT_POS_Y_RATE = 0.82f;		// 次のアイテムのY座標 (画面の高さに対する割合)
	constexpr ConstVector2 NEXT_SIZE = ConstVector2(150.0f, 150.0f);	// 次のアイテムの表示サイズ
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CNextInfo::CNextInfo(const int nPriority) : CObject(nPriority)
{ // タイプ指定
	SetType(TYPE::NEXTINFO);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CNextInfo::~CNextInfo()
{
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CNextInfo *CNextInfo::Create(void)
{
	CNextInfo *pObject = new CNextInfo;		// 生成したオブジェクト
	if (pObject != nullptr)
	{ // nullではないなら初期化
		pObject->Init();
	}

	return pObject;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CNextInfo::Init(void)
{
	// ベースのUIの設定
	CObject2D::Info info;
	info.pos = Vector3(SCREEN_MIDDLE.x, SCREEN_SIZE.y * BASE_POS_Y_RATE, 0.0f);
	info.size = Vector2(SCREEN_SIZE.x * BASE_WIDTH_RATE, BASE_HEIGHT);
	info.texturePath = NORMAL_BASE;

	// ベースのUIを作成
	m_pBase = CObject2D::Create(info);
	m_fAlpha = 1.0f;		// α値を1に設定
	m_nCountWaitAlphaDecrease = WAIT_ALPHA_DECREASE_FRAME;		// 減少待機フレーム設定
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CNextInfo::Uninit(void)
{
	if (m_pBase != nullptr)
	{ // ベースの破棄
		m_pBase->Uninit();
		m_pBase = nullptr;
	}

	if (m_pNext != nullptr)
	{ // 次に取得するアイテム表示の破棄
		m_pNext->Uninit();
		m_pNext = nullptr;
	}

	// 解放処理
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CNextInfo::Update(void)
{
	if (m_nCountWaitAlphaDecrease < 0)
	{ // 待機フレーム無し
		if (m_fAlpha > MIN_ALPHA) m_fAlpha -= ALPHA_DECREASE;	// α値を徐々に下げる
		m_pBase->SetAlpha(m_fAlpha);
		m_pNext->SetAlpha(m_fAlpha);
	}
	else
	{ // 待機フレーム減少
		m_nCountWaitAlphaDecrease--;
	}

	if (m_nAngryCount < 0) return;		// テクスチャの再設定の繰り返しを防止するために、early return

	// 怒っている最中にα値が減少するのを防ぐ
	m_fAlpha = 1.0f;	// α値を元に戻す
	m_nCountWaitAlphaDecrease = WAIT_ALPHA_DECREASE_FRAME;		// 待機フレーム設定

	m_nAngryCount--;
	if (m_nAngryCount > 0) return;		// 怒りのフレーム数が0以上の場合、スキップ

	// アイテムUIの描画フラグを立てる
	if (m_pNext != nullptr) m_pNext->SetDisp(true);

	// 通常のテクスチャに変更
	m_pBase->BindTexture(CTexture::GetInstance()->Register(NORMAL_BASE));
}

//==================================================================================
// --- 次のアイテム表示ポリゴンの設定処理 ---
//==================================================================================
void CNextInfo::SetNext(CObject2D *pNext)
{
	if (m_pNext != nullptr)
	{ // 次に取得するアイテム表示の破棄
		m_pNext->Uninit();
		m_pNext = nullptr;
	}

	// ポインタがnullの場合、スキップ
	if (pNext == nullptr) return;

	// ポリゴンの設定
	pNext->SetPosition(Vector3(SCREEN_MIDDLE.x * NEXT_POS_X_RATE, SCREEN_SIZE.y * NEXT_POS_Y_RATE, 0.0f));
	pNext->SetSize(NEXT_SIZE);

	// もし起こっている最中なら描画フラグを下ろす
	if (m_nAngryCount > 0) pNext->SetDisp(false);

	m_fAlpha = 1.0f;	// α値を元に戻す
	m_nCountWaitAlphaDecrease = WAIT_ALPHA_DECREASE_FRAME;		// 待機フレーム設定
	m_pNext = pNext;	// ポインタを再設定

	// α値設定
	m_pBase->SetAlpha(m_fAlpha);
	m_pNext->SetAlpha(m_fAlpha);
}

//==================================================================================
// --- 怒り処理 ---
//==================================================================================
void CNextInfo::Angry(const int nAngryFrame)
{
	m_nAngryCount = nAngryFrame;		// フレーム数を設定
	
	if (m_pNext != nullptr)		// nullの場合スキップ
	{ // アイテム表示の描画をやめる
		m_pNext->SetDisp(false);
	}

	// 怒り表示のテクスチャに切り替える
	m_pBase->BindTexture(CTexture::GetInstance()->Register(ANGRY_BASE));
	m_fAlpha = 1.0f;	// α値を元に戻す
	m_nCountWaitAlphaDecrease = WAIT_ALPHA_DECREASE_FRAME;		// 待機フレーム設定

	// α値設定
	m_pBase->SetAlpha(m_fAlpha);
	m_pNext->SetAlpha(m_fAlpha);
}