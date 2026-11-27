//==================================================================================
// 
// ゲームシーンのソースファイル [game.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/game.h"
#include "game/stage/mapLoader.h"
#include "game/item/itemGenerator.h"
#include "game/item/mealManager.h"
#include "game/player/player.h"
#include "game/stage/stage.h"
#include "effect/screenEffect.h"
#include "network/rankingClient.h"
#include "ui/score.h"
#include "sound/sound.h"
#include "render/texture.h"
#include "render/camera.h"
#include "core/manager.h"
#include "core/serviceLocator.h"
#include "input/input.h"
#include "input/joypad.h"
#include "ui/nextInfo.h"
#include "ui/timer.h"
#include "game/stage/foodBackground.h"
#include "game/spring.h"
#include "effect/particleInstancing.h"
#include "util/filestream.h"
#include "math/vec3math.h"
#include "debug/debugproc.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view SERVER_IPADDRESS = "127.0.0.1";		// ランキングサーバーのIPアドレス
	constexpr u_short SERVER_PORT = 22333;							// ランキングサーバーのポート番号
	constexpr std::string_view NUMBER_TEXTURE = "data/TEXTURE/voltNumber003.png";	// 数値のテクスチャ
	constexpr ConstColor DAMAGE_FLASH_COLOR = ConstColor(1.0f, 0.0f, 0.0f, 0.0f);	// スクリーンエフェクトの色
	constexpr float DAMAGE_FLASH_FRAME = 60.0f;		// スクリーンエフェクトの時間 (フレーム)
	constexpr ConstVector3 SCORE_POS = ConstVector3(SCREEN_MIDDLE.x * 1.35f, 30.0f, 0.0f);	// スコアの位置 (左上)
	constexpr ConstVector2 SCORE_SIZE = ConstVector2(350.0f, 70.0f);	// スコアの表示サイズ
	constexpr int SCORE_DIGIT = 8;			// スコアの桁数
}
	
//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGame::CGame()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CGame::~CGame()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CGame::Init(const ManagerContext::SceneContext &sceneContext)
{ // 開始処理
	Start(sceneContext);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CGame::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CGame::Update(const ManagerContext::SceneContext &sceneContext)
{
	CManager *pManager = CManager::GetInstance();
	CInputKeyboard *pKeyboard = CServiceLocator::GetInputKeyboard();
	CJoypad *pJoypad = CServiceLocator::GetJoypad();

	if ((pKeyboard->GetTrigger(DIK_P)
		|| pJoypad->GetTrigger(CJoypad::KEY::START))
		&& pManager->IsTransition() == false)
	{
		pManager->SetEnablePause(!pManager->GetEnablePause());
	}

	// エフェクトの総数表示
	sceneContext.pRenderContext->pDebugProc->ForcePrint("Effect Num : {}\n", CObject::FindAnyObjectByType<CParticleInstancing>()->GetNumEffect());

#ifdef _DEBUG
	if (pKeyboard->GetTrigger(DIK_NUMPAD1))
	{
		CCamera::SetFocus(CCamera::TYPE::FREE);
	}
	else if (pKeyboard->GetTrigger(DIK_NUMPAD2))
	{
		CCamera::SetFocus(CCamera::TYPE::PLAYER);
	}
#endif

	if (CTimer::Find()->GetTimer() <= 0
		&& CManager::GetInstance()->IsTransition() == false)
	{
		auto pRankingClient = std::make_unique<CRankingClient>();
		auto pScore = CObject::FindAnyObjectByType<CScore>();
		if (SUCCEEDED(pRankingClient->Init(SERVER_IPADDRESS, SERVER_PORT)))
		{ // 初期化成功時のみ送信
			pRankingClient->Send(pScore->GetScore());
			pRankingClient->Uninit();
		}

		auto pFile = std::make_unique<CFileStream>();
		if (pFile->Open("data/bin/current.bin", CFileStream::MODE::WRITE, true))
		{ // ファイルに書き出し
			pFile->WriteType(pScore->GetScore());
			pFile->Close();
		}

		// 画面を遷移させる
		CManager::GetInstance()->SetTransition(MODE::RANKING);
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGame::Draw(const ManagerContext::SceneContext &sceneContext)
{
}

//==================================================================================
// --- 開始時処理 ---
//==================================================================================
void CGame::Start(const ManagerContext::SceneContext &sceneContext)
{ // カメラを設置
	CCamera::Create(Vector3(0.0f, 1500.0f, -400.0f), VECTOR3_NULL);
	CCamera::SetFocus(CCamera::TYPE::PLAYER);

	// マップ読み込み
	auto pMapLoader = std::make_unique<CMapLoader>();		// マップ読み込みインスタンス

	// テクスチャクラスをバインド
	pMapLoader->BindTextureLoader(sceneContext.pRenderContext->pTexture);
	auto hr = pMapLoader->Load("data/SCRIPT/system.ini");

	// スクリーンエフェクト生成
	CScreenEffect::Create(DAMAGE_FLASH_COLOR, DAMAGE_FLASH_FRAME);

	// スコア表示生成
	CScore::Create(CServiceLocator::GetTexture()->Register(NUMBER_TEXTURE),
		SCORE_POS,
		SCORE_SIZE,
		SCORE_DIGIT);

	// アイテム生成器起動
	CMealManager::Create("data/SCRIPT/itemSetting.txt", 
		CObject::FindAnyObjectByType<CPlayer>(),
		CObject::FindAnyObjectByType<CStage>());

	// バックの食べ物出現
	CFoodBackground::Create();

	CServiceLocator::GetSound()->Play(CSound::LABEL::BGM_INGAME);

	auto pInst = CParticleInstancing::Create({}, 5000000);
	pInst->BindTexture(sceneContext.pRenderContext->pTexture->Register("data/TEXTURE/effect000.jpg"));
	for (int i = 0; i < 750000; i++)
	{
		pInst->Set(VECTOR3_NULL,
			Vec3::Random() * 1.5f,
			COLOR_ONE,
			5.0f,
			0.0f,
			6000);
	}
}