//==================================================================================
// 
// アイテム生成クラスのソースファイル [itemSpawner.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/itemSpawner.h"
#include "game/item/item.h"
#include "game/stage/stage.h"
#include "util/enumUtil.h"
#include "math/rand.h"
#include "math/vec3math.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int ITEM_LIFE = 1;		// アイテムの体力
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CItemSpawner::CItemSpawner() 
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CItemSpawner::~CItemSpawner()
{
}

//==================================================================================
// --- アイテム生成処理 ---
//==================================================================================
std::vector<CItem*> CItemSpawner::Generate(const int nNumItem, 
	CField *pField, 
	const Vector3 &posPlayer)
{
	CItem::TYPE nextType = EnumUtil::AddEnum(CItem::TYPE::NONE, 1);		// 出現させる食べ物の種類
	std::vector<CItem*> vpItem;		// 生成したアイテム
	Vector4 boader;		// 境界線

	// 境界線を計算
	boader.x = pField->GetPosition()->x + (pField->GetSize()->x * 0.5f) - m_fSpawnFieldMargin;		// 右端
	boader.y = pField->GetPosition()->y + (pField->GetSize()->y * 0.5f) - m_fSpawnFieldMargin;		// 上端
	boader.z = pField->GetPosition()->y - (pField->GetSize()->y * 0.5f) + m_fSpawnFieldMargin;		// 下端
	boader.w = pField->GetPosition()->x - (pField->GetSize()->x * 0.5f) + m_fSpawnFieldMargin;		// 左端

	// アイテムの数分サイズを確保
	vpItem.reserve(nNumItem);

	for (int nCntItem = 0; nCntItem < nNumItem; nCntItem++)
	{ // 引数の回数分アイテム生成
		CRand *pRand = CRand::GetInstance();		// 乱数生成機
		Vector3 pos;		// 生成位置
		float fRadius;		// 当たり判定の半径
		int nLife;			// 体力
		int nCntTry = 0;	// 試行回数

		while (nCntTry > m_nTryIfFailed)
		{ // 指定の回数失敗するまで繰り返し
			nCntTry++;		// 試行回数増加

			// 境界線内のどこかにランダムで生成
			pos.x = pRand->Generate(boader.w, boader.x);
			pos.y = 0.0f;
			pos.z = pRand->Generate(boader.z, boader.y);

			// プレイヤーと近すぎた場合はやり直し
			if (Vec3::Length(pos, posPlayer) < m_fDistanceFromPlayer) continue;

			bool bIsFarOtherItems = true;
			for (CItem *const &pItem : vpItem)
			{ // 各アイテムと距離を取れているか
				if (Vec3::Length(pos, *pItem->GetPosition()) < m_fDistanceFromOtherItem)
				{ // 一定距離未満の場合、フラグを下ろす
					bIsFarOtherItems = false;
				}
			}

			// フラグが降りていた場合はやり直し
			if (bIsFarOtherItems == false) continue;
			break;
		}

		// 位置の計算に失敗し続けた場合、スキップ
		if (nCntItem > m_nTryIfFailed) continue;

		// アイテムを生成
		CItem *pItem = CItem::Create(pos,
			m_fItemRadius,
			ITEM_LIFE,
			nextType);

		// 監視と親を設定
		pItem->SetParent(pField->GetMatrix());

		// ポインタ登録
		vpItem.push_back(pItem);

		nextType = EnumUtil::AddEnum(nextType, 1);		// 次に取得する食事の番号を更新
		if (nextType >= CItem::TYPE::MAX)
		{ // 最後まで言った場合は、最初に戻る
			nextType = EnumUtil::AddEnum(CItem::TYPE::NONE, 1);
		}
	}

	// 生成したアイテムのポインタ配列を返す
	return vpItem;
}