//==================================================================================
// 
// タイトルシーンのソースファイル [title.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/title.h"
#include "object/2d/object2D.h"
#include "object/3d/objectX.h"
#include "core/manager.h"
#include "input/input.h"
#include "input/joypad.h"
#include "render/texture.h"
#include "render/camera.h"
#include "scene/transition/sceneTransition.h"
#include "game/stage/foodBackground.h"
#include "sound/sound.h"
#include "util/util.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view LOGO_PATH = "data/TEXTURE/title_logo.png";		// タイトルロゴ
	constexpr ConstVector3 ROTATE_SPD = ConstVector3(0.0f, 0.01f, 0.005f);		// キャラクターの回転速度
	constexpr ConstColor BACKGROUND_COLOR = ConstColor(1.0f, 1.0f, 1.0f, 0.5f);	// 背景の色
	constexpr ConstVector3 PRESS_ENTER_POS = ConstVector3(SCREEN_MIDDLE.x, 600.0f, 0.0f);	// 「PRESS ENTER」の位置
	constexpr ConstVector2 PRESS_ENTER_SIZE = ConstVector2(300.0f, 50.0f);				// 「PRESS ENTER」のサイズ
	constexpr ConstVector3 LOGO_POS = ConstVector3(SCREEN_MIDDLE.x, 275.0f, 0.0f);	// タイトルロゴの位置
	constexpr ConstVector2 LOGO_SIZE = ConstVector2(650.0f, 550.0f);				// タイトルロゴのサイズ
	constexpr ConstVector3 CAMERA_POS_V = ConstVector3(0.0f, 300.0f, -300.0f);		// カメラの視点
	constexpr ConstVector3 CAMERA_POS_R = ConstVector3(0.0f, 300.0f, 0.0f);			// カメラの注視点
	constexpr ConstVector3 CHARACTER_POS = ConstVector3(50.0f, 300.0f, -200.0f);	// キャラクターの位置
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTitle::CTitle()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTitle::~CTitle()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTitle::Init(const ManagerContext::SceneContext &sceneContext)
{ // 開始処理
	Start(sceneContext);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTitle::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTitle::Update(const ManagerContext::SceneContext &sceneContext)
{
	CInputKeyboard *pKeyboard = sceneContext.pInputSystem->GetKeyboard();
	CJoypad *pJoypad = sceneContext.pInputSystem->GetJoypad();
	bool bIsTransition = sceneContext.pTransition->GetState() != CSceneTransition::STATE::STAY;

	if ((pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY::A)
		|| pJoypad->GetTrigger(CJoypad::KEY::START))
		&& bIsTransition == false)
	{ // 遷移中でなく、Enterを押されたら遷移
		CManager::GetInstance()->GetSound()->Play(CSound::LABEL::SE_BITE);
		sceneContext.pTransition->StartTransition(sceneContext.TRANSITION_FRAME, MODE::GAME);
	}

	if (m_pCharacter == nullptr) return;

	const Vector3 *pRot = m_pCharacter->GetRotation();		// 現在の角度
	m_pCharacter->SetRotation(Vector3(Util::FixedRotation(pRot->x + ROTATE_SPD.x),
		Util::FixedRotation(pRot->y + ROTATE_SPD.y),
		Util::FixedRotation(pRot->z + ROTATE_SPD.z)));
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CTitle::Draw(const ManagerContext::SceneContext &sceneContext)
{
}

//==================================================================================
// --- 開始時処理 ---
//==================================================================================
void CTitle::Start(const ManagerContext::SceneContext &sceneContext)
{
	CObject2D::Info info = {};

	info.pos = SCREEN_MIDDLE;
	info.size = SCREEN_SIZE;

	CObject2D::Create(info)->SetColor(BACKGROUND_COLOR);

	info.pos = PRESS_ENTER_POS;
	info.size = PRESS_ENTER_SIZE;

	CObject2D::Create(info)->BindTexture(CTexture::GetInstance()->Register("data/TEXTURE/pressEnter.png"));
	CCamera::Create(CAMERA_POS_V, CAMERA_POS_R);

	info.pos = LOGO_POS;
	info.size = LOGO_SIZE;
	CObject2D::Create(info)->BindTexture(CTexture::GetInstance()->Register(LOGO_PATH));

	// バックの食べ物出現
	CFoodBackground::Create();

	// 回転させるキャラクターを生成
	m_pCharacter = CObjectX::Create("data/MODEL/quaterni_kun.x", CHARACTER_POS);

	// BGM再生
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL::BGM_TITLE);
}