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
#include "game/stage/mapScriptLoader.h"
#include "game/item/itemGenerator.h"
#include "effect/screenEffect.h"
#include "network/rankingClient.h"
#include "ui/score.h"
#include "sound/sound.h"
#include "render/texture.h"
#include "core/manager.h"
#include "input/input.h"
#include "input/joypad.h"
#include "ui/nextInfo.h"
#include "ui/timer.h"
#include "game/stage/foodBackground.h"
#include "util/filestream.h"

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
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();
	CJoypad *pJoypad = pManager->GetJoypad();

	if ((pKeyboard->GetTrigger(DIK_P)
		|| pJoypad->GetTrigger(CJoypad::KEY::START))
		&& pManager->IsTransition() == false)
	{
		pManager->SetEnablePause(!pManager->GetEnablePause());
	}

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

		auto pFile = std::make_unique<CFileStream>("data/bin/current.bin", true, CFileStream::FLAG::OVERWRITE);
		if (pFile->IsOpen())
		{ // ファイルに書き出し
			pFile->Write(pScore->GetScore());
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
{ // マップ読み込み
	std::unique_ptr pMapScriptLoader = std::make_unique<CMapScriptLoader>();		// マップ読み込みインスタンス
	auto hr = pMapScriptLoader->Load("data/SCRIPT/system.ini");

	// アイテム生成器起動
	(new CItemGenerator)->Init();

	// バックの食べ物出現
	CFoodBackground::Create();

	// スクリーンエフェクト生成
	CScreenEffect::Create(DAMAGE_FLASH_COLOR, DAMAGE_FLASH_FRAME);

	CScore::Create(CTexture::GetInstance()->Register(NUMBER_TEXTURE),
		SCORE_POS,
		SCORE_SIZE,
		SCORE_DIGIT);

	CNextInfo::Create();

	CManager::GetInstance()->GetSound()->Play(CSound::LABEL::BGM_INGAME);
}