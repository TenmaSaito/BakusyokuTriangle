//==================================================================================
// 
// アイテム生成クラスのソースファイル [itemGenerator.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/itemGenerator.h"
#include "core/manager.h"
#include "sound/sound.h"
#include "effect/screenEffect.h"
#include "ui/nextInfo.h"
#include "object/2d/object2D.h"
#include "ui/score.h"
#include "game/player/player.h"
#include "game/item/item.h"
#include "game/stage/field.h"
#include "math/rand.h"
#include "math/vec3math.h"
#include "util/util.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int MAX_ITEM_NUM = 15;		// 一度に出現するアイテムの総数
	constexpr std::string_view FOOD_TEXPATHS[static_cast<UINT>(CItem::TYPE::MAX)] =
	{
		"",
		"data/TEXTURE/foods/rice.png",			// ご飯
		"data/TEXTURE/foods/hamburg.png",		// ハンバーグ
		"data/TEXTURE/foods/miso_soup.png",		// みそ汁
	};
	constexpr float SPAWN_FIELD_MARGIN = 70.0f;			// 出現範囲を床の端から内側に縮める量
	constexpr float MIN_DISTANCE_FROM_PLAYER = 300.0f;		// プレイヤーとの最低距離
	constexpr float MIN_DISTANCE_BETWEEN_ITEMS = 200.0f;	// 他のアイテムとの最低距離
	constexpr float ITEM_RADIUS = 100.0f;		// アイテムの当たり判定の半径
	constexpr int ITEM_LIFE_MIN = 1;			// アイテムの体力の最小値
	constexpr int ITEM_LIFE_MAX = 1;			// アイテムの体力の最大値
	constexpr int COMBO_BASE_SCORE = 50;		// コンボ時の基本スコア
	constexpr float COMBO_SCORE_RATE = 1.1f;	// コンボ1回あたりのスコア倍率
	constexpr int FIRST_SPAWN_NUM = 1;			// 最初に出現する食べ物の数
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CItemGenerator::CItemGenerator(const int nPriority) : CObject(nPriority)
{ // タイプ指定
	SetType(TYPE::ITEM_GENERATOR);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CItemGenerator::~CItemGenerator()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CItemGenerator::Init(void)
{ // 各メンバ変数を初期化
	m_nMaxItem = FIRST_SPAWN_NUM;
	m_nextType = CItem::TYPE::RICE;
	m_bWillSetNext = true;
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CItemGenerator::Uninit(void)
{ // 親クラスの終了処理
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CItemGenerator::Update(void)
{ 
	CPlayer *pPlayer = FindAnyObjectByType<CPlayer>(PLAYER_PRIORITY);

	if (m_pField == nullptr)
	{ // 床を取得していなかったら取得
		m_pField = FindAnyObjectByType<CField>(FIELD_PRIORITY);
	}
	
	if (m_bWillSetNext == true)
	{ // 次の食べ物の表示の更新を行うフラグが立っていれば
		CObject2D::Info info = {};
		info.texturePath = FOOD_TEXPATHS[static_cast<UINT>(m_nextType)];

		// 次に食べる食べ物のテクスチャを設定
		FindAnyObjectByType<CNextInfo>(NEXTINFO_PRIORITY)->SetNext(CObject2D::Create(info));
		m_bWillSetNext = false;		// フラグを下ろす
	}

	// アイテムが最大数分生成されているなら、
	// もしくは床へのポインタがnullならスキップ
	if (m_nNumItem > MAX_ITEM_NUM || m_pField == nullptr || pPlayer == nullptr) return;

	// 生成済みの場合スキップ
	if (m_bWasCreated == true) return;

	const Vector3 vecOrigin = VECTOR3_NULL;		// ローカル座標の原点
	Vector3 posPlayer = *D3DXVec3TransformCoord(&posPlayer, &vecOrigin, pPlayer->GetMatrix());

	CItem::TYPE nextType = Util::AddEnum(CItem::TYPE::NONE, 1);		// 出現させる食べ物の種類

	for (int nCntItem = 0; nCntItem < m_nMaxItem; nCntItem++)
	{
		CRand *pRand = CRand::GetInstance();		// 乱数生成機
		Vector3 pos;		// 生成位置
		float fRadius;		// 当たり判定の半径
		int nLife;			// 体力

		Vector4 boader;		// 境界線

		// 境界線を計算
		boader.x = m_pField->GetPosition()->x + (m_pField->GetSize()->x * 0.5f) - SPAWN_FIELD_MARGIN;		// 右端
		boader.y = m_pField->GetPosition()->y + (m_pField->GetSize()->y * 0.5f) - SPAWN_FIELD_MARGIN;		// 上端
		boader.z = m_pField->GetPosition()->y - (m_pField->GetSize()->y * 0.5f) + SPAWN_FIELD_MARGIN;		// 下端
		boader.w = m_pField->GetPosition()->x - (m_pField->GetSize()->x * 0.5f) + SPAWN_FIELD_MARGIN;		// 左端

		while (1)
		{
			// 境界線内のどこかにランダムで生成
			pos.x = pRand->Generate(boader.w, boader.x);
			pos.y = 0.0f;
			pos.z = pRand->Generate(boader.z, boader.y);

			// プレイヤーと近すぎた場合はやり直し
			if (Vec3::Length(pos, posPlayer) < MIN_DISTANCE_FROM_PLAYER) continue;

			bool bIsFarOtherItems = true;
			for (CItem *const &pItem : m_vpItem)
			{ // 各アイテムと距離を取れているか
				if (Vec3::Length(pos, *pItem->GetPosition()) < MIN_DISTANCE_BETWEEN_ITEMS)
				{ // 一定距離未満の場合、フラグを下ろす
					bIsFarOtherItems = false;
				}
			}

			// フラグが降りていた場合はやり直し
			if (bIsFarOtherItems == false) continue;

			break;
		}

		// 半径 + 体力を設定
		fRadius = ITEM_RADIUS;
		nLife = pRand->Generate(ITEM_LIFE_MIN, ITEM_LIFE_MAX);

		// アイテムを生成
		CItem *pItem = CItem::Create(pos,
			fRadius,
			nLife,
			nextType);

		// 監視と親を設定
		pItem->AddObserver(this);
		pItem->SetParent(m_pField->GetMatrix());
		m_nNumItem++;		// アイテムの総数増加

		// ポインタ登録
		m_vpItem.push_back(pItem);

		nextType = Util::AddEnum(nextType, 1);		// 次に取得する食事の番号を更新
		if (nextType >= CItem::TYPE::MAX)
		{ // 最後まで言った場合は、最初に戻る
			nextType = Util::AddEnum(CItem::TYPE::NONE, 1);
		}
	}

	// フラグを立てる
	m_bWasCreated = true;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CItemGenerator::Draw(void)
{
}

//==================================================================================
// --- 通知受け取り処理 ---
//==================================================================================
void CItemGenerator::Notified(CObject *pObject, std::string_view message, const std::any &args)
{ // アイテム以外からの通知以外は無視する
	if (pObject->GetType() != TYPE::ITEM) return;
	// 死亡メッセージ以外は無視
	else if (message != NOTIFY_WHEN_DEATH) return;
	// 未生成の場合はスキップ
	else if (m_bWasCreated == false || m_nNumItem <= 0) return;

	CItem *pItem = static_cast<CItem*>(pObject);		// キャスト

	// 取得すべきアイテムを取得したかチェック
	bool bIsEqual = pItem->IsEqualType(m_nextType);
	if (bIsEqual == true)
	{ // 正しければ、取得時処理を行う
		m_nextType = Util::AddEnum(m_nextType, 1);		// 次に取得する食事の番号を更新
		if (m_nextType >= CItem::TYPE::MAX)
		{ // 最後まで言った場合は、最初に戻る
			m_nextType = Util::AddEnum(CItem::TYPE::NONE, 1);
		}

		m_nNumItem--;		// 総数減少

		// スコアを加算
		if (m_pScore == nullptr)
		{ // 未取得なら、取得する
			m_pScore = FindAnyObjectByType<CScore>(SCORE_PRIORITY);
		}
		m_pScore->AddScore(COMBO_BASE_SCORE * (COMBO_SCORE_RATE * (m_nCombo + 1)));

		// スクリーンエフェクト起動
		if (m_pScreenEffect == nullptr)
		{ // 未取得なら、取得する
			m_pScreenEffect = FindAnyObjectByType<CScreenEffect>(SCREEN_EFFECT_PRIORITY);
		}
		m_pScreenEffect->SetColor(Color(1.0f, 1.0f, 0.0f, 0.0f));
		m_pScreenEffect->Flash(0.5f);

		if (m_nNumItem <= 0)
		{ // 総数が0を下回ったら、生成数を1増やして再度生成 (上限は超えない)
			m_nMaxItem = (m_nMaxItem + 1 > MAX_ITEM_NUM) ? MAX_ITEM_NUM - 1 : m_nMaxItem + 1;
			m_bWasCreated = false;		// 生成フラグを立てる
			m_nextType = Util::AddEnum(CItem::TYPE::NONE, 1);			// 次の種類をリセット

			// ポインタの登録解除
			m_vpItem.erase(std::ranges::find(m_vpItem, pItem));
			std::for_each(m_vpItem.begin(), m_vpItem.end(), [](CItem *&pItem) {
				if (pItem != nullptr)
				{
					pItem->Uninit();
					pItem = nullptr;
				}});

			// 配列をクリア
			m_vpItem.clear();
		}
		else
		{ // ポインタの登録解除
			m_vpItem.erase(std::ranges::find(m_vpItem, pItem));
		}

		CManager::GetInstance()->GetSound()->Play(CSound::LABEL::SE_SUCCESS);
		m_bWillSetNext = true;
		m_nCombo++;
		return;
	}

	// スクリーンエフェクト起動
	if (m_pScreenEffect == nullptr)
	{ // 未取得なら、取得する
		m_pScreenEffect = FindAnyObjectByType<CScreenEffect>(SCREEN_EFFECT_PRIORITY);
	}
	m_pScreenEffect->SetColor(Color(1.0f, 0.0f, 0.0f, 0.0f));
	m_pScreenEffect->Flash(0.5f);

	// コンボや現在のフェーズをリセットする
	m_nCombo = 0;		// コンボ数リセット
	m_nMaxItem = 1;		// 生成数を1にリセット
	m_bWasCreated = false;		// 生成フラグを立てる
	m_nextType = Util::AddEnum(CItem::TYPE::NONE, 1);;			// 次の種類をリセット
	m_nNumItem = 0;				// 総数をリセット

	// ポインタの登録解除
	std::for_each(m_vpItem.begin(), m_vpItem.end(), [](CItem *&pItem) {
		if (pItem != nullptr)
		{
			pItem->Uninit();
			pItem = nullptr;
		}});

	// 配列をクリア
	m_vpItem.clear();

	// 怒らせる
	FindAnyObjectByType<CNextInfo>(NEXTINFO_PRIORITY)->Angry();
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL::SE_FAIL);
	m_bWillSetNext = true;		// 次の食べ物の表示を行うフラグを立てる
}