//==================================================================================
//
// アイテム関連処理の管理クラスのソースファイル [mealManager.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/mealManager.h"
#include "game/item/itemSpawner.h"
#include "game/item/eatingRule.h"
#include "game/item/eatingDirector.h"
#include "game/item/itemLoader.h"
#include "game/item/item.h"
#include "game/player/player.h"
#include "game/stage/stage.h"
#include "render/camera.h"
#include "core/manager.h"
#include "core/serviceLocator.h"
#include "sound/sound.h"
#include "ui/score.h"
#include "effect/screenEffect.h"
#include "util/util.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CMealManager *CMealManager::Create(std::string_view sPath,
	CPlayer *pPlayer,
	CStage *pStage)
{
	return Util::Create<CMealManager>(sPath, pPlayer, pStage);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMealManager::CMealManager() : CObject(MEAL_MANAGER_PRIORITY)
{ // タイプ指定
	SetType(TYPE::CAMERA_SHOT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMealManager::~CMealManager()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMealManager::Init(std::string_view sPath,
	CPlayer *pPlayer,
	CStage *pStage)
{ // null参照防止
	if (pPlayer == nullptr || pStage == nullptr) return E_POINTER;

	// 引数代入
	m_pPlayer = pPlayer;
	m_pStage = pStage;

	// 各アイテム処理インスタンスを生成
	m_pItemSpawner = std::make_unique<CItemSpawner>();
	m_pEatingRule = std::make_unique<CEatingRule>();
	m_pEatingDirector = std::make_unique<CEatingDirector>();

	// スクリプトを読み込み、スポナーとルールに反映
	CItemLoader loader;
	loader.BindItemSpawner(m_pItemSpawner.get());
	loader.BindEatingRule(m_pEatingRule.get());
	if (FAILED(loader.Load(sPath))) return E_FAIL;

	// ディレクターを設定
	SetupDirector();

	// 最初のウェーブのアイテムを生成
	SpawnItems(m_pEatingRule->GetMaxItem());

	// 予告ポリゴンの生成
	m_pEatingDirector->Play(CEatingRule::EAT_RESULT::INIT,
		m_pEatingRule->GetNext(),
		0);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMealManager::Uninit(void)
{ // 残っているアイテムから監視を解除 (解放後に通知が来るのを防ぐ)
	for (CItem *pItem : m_vpItem)
	{
		pItem->RemoveObserver(this);
	}
	m_vpItem.clear();

	// 親クラスの解放
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMealManager::Update(const ManagerContext::RenderContext &context)
{ // 生成要求が無い場合スキップ
	if (m_bSpawnRequest == false) return;

	// 次のウェーブのアイテムを生成
	m_bSpawnRequest = false;
	SpawnItems(m_pEatingRule->GetMaxItem());
}

//==================================================================================
// --- 通知処理 ---
//==================================================================================
void CMealManager::Notified(CObject *pObject,
	std::string_view message,
	const std::any &args)
{ // アイテムの死亡通知以外はスキップ
	if (pObject->GetType() != TYPE::ITEM) return;
	if (message != NOTIFY_WHEN_DEATH) return;

	CItem *pItem = static_cast<CItem*>(pObject);		// アイテムへキャスト

	// 管理配列から削除
	std::erase(m_vpItem, pItem);

	// 取得結果を判定し、演出担当に渡す
	const CEatingRule::EAT_RESULT result = m_pEatingRule->OnEat(pItem->GetItemType());
	m_pEatingDirector->Play(result,
		m_pEatingRule->GetNext(),
		m_pEatingRule->GetCurrentScore());

	if (result == CEatingRule::EAT_RESULT::WRONG)
	{ // 誤食時、残りのアイテムを全て消して、次のUpdateで最初から生成し直す
		DestroyAllItems();
		m_bSpawnRequest = true;
	}
	else if (result == CEatingRule::EAT_RESULT::WAVE_CLEAR)
	{ // ウェーブクリア時、次のUpdateで生成する
		m_bSpawnRequest = true;
	}
}

//==================================================================================
// --- 残りのアイテムの全破棄処理 ---
//==================================================================================
void CMealManager::DestroyAllItems(void)
{
	for (CItem *pItem : m_vpItem)
	{ // 監視を解除してから破棄 (取得扱いの通知が来ないようにする)
		pItem->RemoveObserver(this);
		pItem->Uninit();
	}
	m_vpItem.clear();
}

//==================================================================================
// --- アイテムの演出クラスの設定処理 ---
//==================================================================================
void CMealManager::SetupDirector(void)
{ // 各インスタンス割り当て
	m_pEatingDirector->BindCameras(CCamera::GetCamera(CCamera::TYPE::PLAYER),
		CCamera::GetCamera(CCamera::TYPE::FREE));
	m_pEatingDirector->BindScore(CObject::FindAnyObjectByType<CScore>());
	m_pEatingDirector->BindScreenEffect(CObject::FindAnyObjectByType<CScreenEffect>());
	m_pEatingDirector->BindSound(CServiceLocator::GetSound());
}

//==================================================================================
// --- アイテムの生成・監視登録処理 ---
//==================================================================================
void CMealManager::SpawnItems(const int nNumItem)
{ // アイテムを生成
	m_vpItem = m_pItemSpawner->Generate(nNumItem, m_pStage, *m_pPlayer->GetPosition());
	if (m_vpItem.size() != nNumItem)
	{ // 生成数が異なっていた場合、詰み防止のため総数を修正
		m_pEatingRule->FixMaxItem(m_vpItem.size());
	}

	for (CItem *pItem : m_vpItem)
	{ // 死亡通知を受け取るため、オブザーバーに登録
		pItem->AddObserver(this);
	}
}