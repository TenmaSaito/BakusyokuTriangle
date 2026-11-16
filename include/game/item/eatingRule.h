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
		MAX
	};

	CEatingRule();
	~CEatingRule();

	EAT_RESULT OnEat(const CItem::TYPE eat);
	void SetItemScore(const CItem::TYPE type, const int nScore) { m_aItemScore[ConvertToArrayIndex(type)] = nScore; }
	int GetScore(void) const { return m_nScore; }
	CItem::TYPE GetNext(void) const { return m_nextType; }

private:
	int ConvertToArrayIndex(const CItem::TYPE type) const { return static_cast<int>(type) - 1; }

	std::array<int, CItem::GetTypeMax()> m_aItemScore = {};		// 各アイテムのスコア
	CItem::TYPE m_nextType = CItem::TYPE::NONE;		// 次に取るべき食事の種類
	int m_nScore = 0;		// 現在のスコア
	int m_nMaxItem = 0;		// 現在のフェーズで出現するアイテムの数
	int m_nNumEatItem = 0;	// 取得したアイテムの総数
};
#endif