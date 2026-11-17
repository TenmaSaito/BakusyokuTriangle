//==================================================================================
// 
// アイテム生成クラスのヘッダーファイル [itemSpawner.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _ITEM_SPAWNER_H_		// インクルードガード
#define _ITEM_SPAWNER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <functional>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CField;
class CItem;

//**********************************************************************************
// *** アイテム生成クラス ***
//**********************************************************************************
class CItemSpawner
{
public:
	CItemSpawner();
	~CItemSpawner();

	std::vector<CItem*> Generate(const int nNumItem,
		CField *pField,
		const Vector3 &posPlayer);
	void SetSpawnFieldMargin(const float fValue) { m_fSpawnFieldMargin = fValue; }
	void SetDistanceFromPlayer(const float fValue) { m_fDistanceFromPlayer = fValue; }
	void SetDistanceFromOtherItem(const float fValue) { m_fDistanceFromOtherItem = fValue; }
	void SetItemRadius(const float fValue) { m_fItemRadius = fValue; }
	void SetTryIfFailed(const int nValue) { m_nTryIfFailed = nValue; }

private:
	float m_fSpawnFieldMargin = 0.0f;			// ステージの端から離す距離
	float m_fDistanceFromPlayer = 0.0f;			// アイテムとプレイヤーの最短距離
	float m_fDistanceFromOtherItem = 0.0f;		// アイテム同士の最短距離
	float m_fItemRadius = 0.0f;					// アイテムの当たり判定の半径
	int m_nTryIfFailed = 0;		// 距離制限による設置失敗時に行う試行回数の最大
};

#endif