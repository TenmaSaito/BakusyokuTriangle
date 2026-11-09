//==================================================================================
// 
// ステージクラスのソースファイル [stage.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/stage/stage.h"
#include "core/manager.h"
#include "input/input.h"
#include "input/joypad.h"
#include "input/inputSystem.h"
#include "input/inputAction.h"
#include "util/util.h"
#include "math/vec2math.h"
#include "game/player/player.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float LEAN_MIN = -QUARTER_PI * 0.5f;		// 傾きの最小
	constexpr float LEAN_MAX = QUARTER_PI * 0.5f;		// 傾きの最大
	constexpr float LEAN_POW = 0.05f;		// 傾ける強さ
	constexpr float LEAN_RESIST = 0.1f;		// 傾きの元に戻る強さ
	constexpr float ROLL_FORCE = 0.03f;		// 転がす強さ
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CStage *CStage::Create(const Vector3 &pos,
	const Vector3 &rot,
	const Vector2 &size)
{
	CStage *pStage = new CStage;		// 生成したオブジェクトへのポインタ
	if (pStage != nullptr)
	{ // 初期化処理
		pStage->Init(pos, rot, size);
	}

	return pStage;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CStage::CStage(const int nPriority) : CField(nPriority)
{ // タイプを指定
	SetType(TYPE::FIELD);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CStage::~CStage()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CStage::Init(const Vector3 &pos,
	const Vector3 &rot,
	const Vector2 &size)
{ // 親クラスの初期化
	return CField::Init(pos, rot, size);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CStage::Uninit(void)
{ 
	if (m_pPlayer != nullptr)
	{ // プレイヤーの監視を終了する
		m_pPlayer->RemoveObserver(this);
	}
	
	// 親クラスの終了処理
	CField::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CStage::Update(void)
{
	Vector3 rot = *GetRotation();		// 現在の角度
	bool bPush = false;			// 入力されたか

	if (m_pPlayer == nullptr)
	{ // プレイヤーを傾けるため、ポインタがnullなら取得
		m_pPlayer = FindAnyObjectByType<CPlayer>();

		// プレイヤー死亡後にポインタを保持し続けるのを防ぐため、監視
		m_pPlayer->AddObserver(this);
		m_pPlayer->SetParent(GetMatrix());		// 親マトリックスを適用
	}

	// 傾き入力処理
	m_bLeanInput = LeanInput(&rot);
	if (m_bLeanInput == true)
	{ // 傾けた場合、その向きにプレイヤーを転がす
		float fAngle = Vec2::Direction(Vector2(rot.z / LEAN_MAX, -rot.x / LEAN_MAX), VECTOR2_NULL);	// 傾けた向き
		float fForce = Vec2::Length(Vector2(rot.z / LEAN_MAX, rot.x / LEAN_MAX)) * ROLL_FORCE;		// 転がす強さ

		// プレイヤーを転がす
		m_pPlayer->AddForce(fAngle, fForce);
	}

	// 傾きを適用する
	SetRotation(rot);
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CStage::Draw(void)
{ // 親クラスの描画処理
	CField::Draw();
}

//==================================================================================
// --- 通知処理 ---
//==================================================================================
void CStage::Notified(CObject *pObject, std::string_view message, const std::any &args)
{ // ポインタがプレイヤーへのポインタでは無かったら、スキップ
	if (pObject != m_pPlayer) return;
	// 死亡メッセージでは無かったならスキップ
	else if (message != NOTIFY_WHEN_DEATH) return;

	m_pPlayer = nullptr;		// プレイヤーが死亡したため、ポインタをnullにする
}

//==================================================================================
// --- 傾き入力処理 ---
//==================================================================================
bool CStage::LeanInput(Vector3 *pRot)
{
	CManager *pManager = CManager::GetInstance();					// マネージャー
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボード
	CJoypad *pJoypad = pManager->GetJoypad();						// ジョイパッド
	CInputSystem *pSystem = CInputSystem::GetInstance();
	auto pVertical = pSystem->FindAction("Legacy", "Vertical");
	auto pHorizontal = pSystem->FindAction("Legacy", "Horizontal");
	bool bPush = false;			// 入力されたか

	float fValueVertical = pVertical->ReadValue1D();
	float fValueHorizontal = pHorizontal->ReadValue1D() * -1;

	if (pVertical->IsPressed())
	{
		pRot->x += fValueVertical * LEAN_POW;
		pRot->x = std::clamp(pRot->x, LEAN_MIN, LEAN_MAX);
		bPush = true;		// 入力フラグを立てる
	}
	else
	{ // 前後の台の傾きを元に戻す
		pRot->x += (0.0f - pRot->x) * LEAN_RESIST;
	}

	if (pHorizontal->IsPressed())
	{
		pRot->z += fValueHorizontal * LEAN_POW;
		pRot->z = std::clamp(pRot->z, LEAN_MIN, LEAN_MAX);
		bPush = true;		// 入力フラグを立てる
	}
	else
	{ // 左右の台の傾きを元に戻す
		pRot->z += (0.0f - pRot->z) * LEAN_RESIST;
	}

#if 0
	// 前後の傾き入力
	if (pKeyboard->GetPress(DIK_W))
	{ // 台を前に傾ける
		pRot->x += LEAN_POW;
		pRot->x = std::clamp(pRot->x, LEAN_MIN, LEAN_MAX);		
		bPush = true;		// 入力フラグを立てる
	}
	else if (pKeyboard->GetPress(DIK_S))
	{ // 台を後ろに傾ける
		pRot->x -= LEAN_POW;
		pRot->x = std::clamp(pRot->x, LEAN_MIN, LEAN_MAX);
		bPush = true;		// 入力フラグを立てる
	}
	else
	{ // 前後の台の傾きを元に戻す
		pRot->x += (0.0f - pRot->x) * LEAN_RESIST;
	}

	// 左右の傾き入力
	if (pKeyboard->GetPress(DIK_A))
	{ // 台を左に傾ける
		pRot->z += LEAN_POW;
		pRot->z = std::clamp(pRot->z, LEAN_MIN, LEAN_MAX);
		bPush = true;		// 入力フラグを立てる
	}
	else if (pKeyboard->GetPress(DIK_D))
	{ // 台を右に傾ける
		pRot->z -= LEAN_POW;
		pRot->z = std::clamp(pRot->z, LEAN_MIN, LEAN_MAX);
		bPush = true;		// 入力フラグを立てる
	}
	else
	{ // 左右の台の傾きを元に戻す
		pRot->z += (0.0f - pRot->z) * LEAN_RESIST;
	}
#endif

	// 入力されたかを返す
	return bPush;
}