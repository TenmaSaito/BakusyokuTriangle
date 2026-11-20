//==================================================================================
// 
// アイテム取得のルール管理クラスのヘッダーファイル [eatingRule.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _EATING_RULE_H_		// インクルードガード
#define _EATING_RULE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/item.h"

//**********************************************************************************
// *** アイテム取得のルール管理クラス ***
//**********************************************************************************
class CEatingRule
{
public:
	// 取得結果
	enum class EAT_RESULT
	{
		SUCCESS = 0,		// 取得成功
		WRONG,				// 取得失敗
		WAVE_CLEAR,			// ウェーブクリア
		INIT,				// 初期化時生成
		MAX
	};

	CEatingRule();
	~CEatingRule();

	EAT_RESULT OnEat(const CItem::TYPE eat);
	void SetItemScore(const CItem::TYPE type, const int nScore) { m_aItemScore[CItem::ConvertToArrayIndex(type)] = nScore; }
	int GetScore(void) const { return m_nScore; }
	int GetCurrentScore(void) const { return m_nCurrentScore; }
	int GetMaxItem(void) const { return m_nMaxItem; }
	CItem::TYPE GetNext(void) const { return m_nextType; }

private:
	std::array<int, CItem::GetTypeMax()> m_aItemScore = {};		// 各アイテムのスコア
	CItem::TYPE m_nextType = CItem::TYPE::RICE;		// 次に取るべき食事の種類
	int m_nScore = 0;			// 現在のスコア
	int m_nCurrentScore = 0;	// 直近に獲得したスコア
	int m_nCombo = 0;		// 現在のコンボ数
	int m_nMaxItem = 1;		// 現在のフェーズで出現するアイテムの数
	int m_nNumEatItem = 0;	// 取得したアイテムの総数
};
#endif