//==================================================================================
// 
// ポーズ画面クラスのソースファイル [pause.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/3
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "ui/pause.h"
#include "render/texture.h"
#include "object/2d/polygon2D.h"
#include "math/vec3math.h"
#include "core/manager.h"
#include "sound/sound.h"
#include "input/input.h"
#include "input/joypad.h"
#include "scene/transition/sceneTransition.h"
#include "util/util.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
constexpr ConstVector2 DEFAULT_SIZE = ConstVector2(200.0f, 200.0f);		// メニューの基本サイズ
constexpr float MENU_SCALE_MAGNI = 1.5f;			// カーソルと当たった時のサイズ拡大倍率
constexpr ConstColor BACK_COLOR = ConstColor(0.0f, 0.0f, 0.0f, 0.5f);	// ポーズ中の背景ポリゴンの色

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asPausePath[EnumUtil::ToIndex(CPause::TYPE::MAX)] =
	{ // ポーズ画面の各メニューのテクスチャパス
		"data/TEXTURE/pause/continue.png",		// 続行
		"data/TEXTURE/pause/restart.png",		// やり直し
		"data/TEXTURE/pause/exit.png",			// 終了
	};

	constexpr ConstVector3 c_aPosMenu[EnumUtil::ToIndex(CPause::TYPE::MAX)] =
	{ // ポーズ画面の各メニューのポリゴンの座標
		ConstVector3(240.0f, 320.0f, 0.0f),
		ConstVector3(640.0f, 320.0f, 0.0f),
		ConstVector3(1040.0f, 320.0f, 0.0f),
	};
	constexpr int STICK_REPEAT_WAIT = 5;		// スティックのリピート入力の待機時間
	constexpr int STICK_REPEAT_INTERVAL = 15;	// スティックのリピート入力の間隔
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPause *CPause::Create(void)
{
	CPause *pPause = new CPause;		// 生成したポーズへのポインタ
	if (pPause != nullptr)
	{ // 生成されていれば初期化
		pPause->Init();
	}

	return pPause;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPause::CPause()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPause::~CPause()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPause::Init(void)
{
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理オブジェクトへのポインタ

	for (int nCntPoly = 0; nCntPoly < EnumUtil::ToUnderlying(TYPE::MAX); nCntPoly++)
	{ // 各テクスチャを読み込み
		// 各メニューのポリゴンを作成
		m_apMenu[nCntPoly].reset(CPolygon2D::Create(c_aPosMenu[nCntPoly], VECTOR3_NULL, DEFAULT_SIZE));

		// テクスチャを紐づけ
		m_apMenu[nCntPoly]->BindTexture(pTexture->Register(c_asPausePath[nCntPoly]));
	}

	// 背景ポリゴンを生成
	m_pBack.reset(CPolygon2D::Create(SCREEN_MIDDLE, VECTOR3_NULL, SCREEN_SIZE));
	m_pBack->SetColor(BACK_COLOR);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPause::Uninit(void)
{
	if (m_pBack != nullptr)
	{ // ポリゴンを破棄 (nullならスキップ)
		m_pBack->Uninit();
		m_pBack.reset();
	}

	for (auto &pPoly : m_apMenu)
	{ // ポリゴンを破棄 (nullならスキップ)
		if (pPoly == nullptr) continue;

		pPoly->Uninit();
		pPoly.reset();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPause::Update(void)
{ 
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ

	// 描画フラグが立っていない、もしくは既にモード移行が始まっているならスキップ
	if (m_bDisp != true || pManager->IsTransition() == true) return;

	// 入力関連の更新
	UpdateInput();

	for (int nCntType = 0; nCntType < EnumUtil::ToUnderlying(TYPE::MAX); nCntType++)
	{ // タイプ分繰り返し
		if (nCntType == EnumUtil::ToUnderlying(m_type))
		{ // 当たっていれば、サイズを拡大 + 現在のタイプを変更
			m_apMenu[nCntType]->SetSize(DEFAULT_SIZE * MENU_SCALE_MAGNI);
		}
		else
		{ // 当たっていなければ、サイズを戻す + タイプを無効値に設定
			m_apMenu[nCntType]->SetSize(DEFAULT_SIZE);
		}
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPause::Draw(void)
{ // 描画フラグが降りている場合、スキップ
	if (m_bDisp != true) return;

	// 背景ポリゴンを先に描画
	if(m_pBack != nullptr) m_pBack->Draw();

	for (auto &pPoly : m_apMenu)
	{ // ポリゴンを描画 (nullならスキップ)
		if (pPoly == nullptr) continue;

		pPoly->Draw();
	}
}

//==================================================================================
// --- 入力関連の更新処理 ---
//==================================================================================
void CPause::UpdateInput(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CInputMouse *pMouse = pManager->GetInputMouse();				// マウスへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();						// ジョイパッドへのポインタ
	CSound *pSound = pManager->GetSound();				// サウンドへのポインタ
	Vector3 mousePos = pMouse->GetPositionInClient();	// マウス座標

	// タイプが範囲になった場合、アサーション
	assert(IsValid(m_type));

	if (pKeyboard->GetTrigger(DIK_D)
		|| pJoypad->GetTrigger(CJoypad::KEY::RIGHT)
		|| pJoypad->GetStickRepeat(CJoypad::STICK::LEFT_RIGHT, 
			JOYPAD_DEFAULT_STICK_RANGE,
			STICK_REPEAT_WAIT,
			STICK_REPEAT_INTERVAL))
	{ // ユーザーが右入力をした際、タイプを進める
		m_type = Util::AddEnum(m_type, 1);
		if (m_type >= TYPE::MAX) m_type = TYPE::CONTINUE;
	}
	else if (pKeyboard->GetTrigger(DIK_A)
		|| pJoypad->GetTrigger(CJoypad::KEY::LEFT)
		|| pJoypad->GetStickRepeat(CJoypad::STICK::LEFT_LEFT,
			JOYPAD_DEFAULT_STICK_RANGE,
			STICK_REPEAT_WAIT,
			STICK_REPEAT_INTERVAL))
	{ // ユーザーが左入力をした際、タイプを戻す
		m_type = Util::AddEnum(m_type, -1);
		if (m_type < TYPE::CONTINUE) m_type = TYPE::EXIT;
	}
	else if (pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY::A)
		|| (pMouse->GetTrigger(CInputMouse::KEY::LEFT) && m_apMenu[EnumUtil::ToIndex(m_type)]->Collision(mousePos)))
	{ // ユーザーが決定ボタンを押したとき (Enter || A || 左クリック)、今のタイプで処理実行
		ActionType();
	}
}

//==================================================================================
// --- タイプ別処理 ---
//==================================================================================
void CPause::ActionType(void)
{ // 現在のタイプの処理を実行
	ExecuteType(m_type);
}

//==================================================================================
// --- 指定タイプの処理実行 ---
//==================================================================================
void CPause::ExecuteType(const TYPE type)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ

	// 指定されたタイプで処理を分ける
	switch (type)
	{
	case TYPE::CONTINUE:		// ゲーム再開 (ポーズ解除)
		m_bDisp = false;	// 描画フラグを下ろす
		break;

	case TYPE::RESTART:		// リスタート (現在のシーン読み込み直し)
		pManager->SetTransition(pManager->GetMode());
		pManager->GetSound()->Play(CSound::LABEL::SE_BITE);
		break;

	case TYPE::EXIT:			// タイトルへ戻る
		pManager->SetTransition(CScene::MODE::TITLE);
		pManager->GetSound()->Play(CSound::LABEL::SE_BITE);
		break;

	default:
		break;
	}
}

//==================================================================================
// --- 選択肢ポリゴンの取得処理 ---
//==================================================================================
const CPolygon2D *CPause::GetMenu(const TYPE type) const
{ // 範囲外ならnullを返す
	if (!IsValid(type)) return nullptr;

	return m_apMenu[EnumUtil::ToIndex(type)].get();
}