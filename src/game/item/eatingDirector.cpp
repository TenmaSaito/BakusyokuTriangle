//==================================================================================
// 
// アイテム取得時演出の管理クラスのソースファイル [item.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/eatingDirector.h"
#include "effect/screenEffect.h"
#include "object/2d/object2D.h"
#include "render/cameraShot.h"
#include "sound/sound.h"
#include "ui/nextInfo.h"
#include "ui/score.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int CAMERA_TRANS_FRAME = 10;		// カメラの各遷移時間
	constexpr int CAMERA_TRANS_WAIT_FRAME = 20;	// カメラの遷移後の待機時間
	constexpr float FLASH_ALPHA = 0.5f;			// フラッシュの強さ
	constexpr ConstColor SUCCESS_COL = ConstColor(1.0f, 1.0f, 0.0f, 1.0f);	// 成功時のフラッシュ色
	constexpr ConstColor FAIL_COL = ConstColor(1.0f, 0.0f, 0.0f, 1.0f);		// 失敗時のフラッシュ色
	constexpr std::string_view FOOD_TEXPATHS[CItem::GetTypeMax()] =			// 各食べ物のテクスチャパス
	{
		"data/TEXTURE/foods/rice.png",			// ご飯
		"data/TEXTURE/foods/hamburg.png",		// ハンバーグ
		"data/TEXTURE/foods/miso_soup.png",		// みそ汁
	};
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CEatingDirector::CEatingDirector()
{ // 次回予告用インスタンス生成
	m_pNextInfo = CNextInfo::Create();
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CEatingDirector::~CEatingDirector()
{
}

//==================================================================================
// --- 演出の再生 ---
//==================================================================================
void CEatingDirector::Play(const CEatingRule::EAT_RESULT result,
	const CItem::TYPE next,
	const int nScore)
{
	if (m_pNextInfo != nullptr)
	{ // 次に表示するポリゴンを作成
		CObject2D::Info info{ .texturePath = FOOD_TEXPATHS[CItem::ConvertToArrayIndex(next)] };
		m_pNextInfo->SetNext(CObject2D::Create(info));
	}

	switch (result)
	{
	case CEatingRule::EAT_RESULT::SUCCESS:			// 成功時
		Success(nScore);
		break;

	case CEatingRule::EAT_RESULT::WRONG:			// 失敗時
		Fail();
		break;

	case CEatingRule::EAT_RESULT::WAVE_CLEAR:		// ウェーブクリア時
		WaveClear(nScore);
		break;

	case CEatingRule::EAT_RESULT::INIT:				// 初期化時
		break;

	default:
		assert(!"ここを通る筈ありません。列挙型の範囲外です。");
		break;
	}
}

//==================================================================================
// --- 成功演出 ---
//==================================================================================
void CEatingDirector::Success(const int nScore)
{ // 演出生成
	SetDirection(static_cast<int>(CSound::LABEL::SE_SUCCESS), SUCCESS_COL);

	if (m_pScore != nullptr)
	{ // スコアを加算
		m_pScore->AddScore(nScore);
	}
}

//==================================================================================
// --- 失敗演出 ---
//==================================================================================
void CEatingDirector::Fail(void)
{ // 演出生成
	SetDirection(static_cast<int>(CSound::LABEL::SE_FAIL), FAIL_COL);

	if (m_pNextInfo != nullptr)
	{ // 怒り演出
		m_pNextInfo->Angry();
	}
}

//==================================================================================
// --- ウェーブクリア演出 ---
//==================================================================================
void CEatingDirector::WaveClear(const int nScore)
{ // 成功時演出を再生
	Success(nScore);

	// カメラを往復遷移
	CCameraShot::Create(m_pCamPlayer,
		m_pCamField,
		CAMERA_TRANS_FRAME,
		CAMERA_TRANS_FRAME,
		CAMERA_TRANS_WAIT_FRAME);
}

//==================================================================================
// --- 演出生成 ---
//==================================================================================
void CEatingDirector::SetDirection(const int nSoundType, const Color col)
{
	if (m_pSound != nullptr)
	{ // 失敗音再生
		m_pSound->Play(static_cast<CSound::LABEL>(nSoundType));
	}

	if (m_pScreenEffect != nullptr)
	{ // 赤いフラッシュ演出
		m_pScreenEffect->SetColor(col);
		m_pScreenEffect->Flash(FLASH_ALPHA);
	}
}