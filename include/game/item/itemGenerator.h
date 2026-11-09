//==================================================================================
// 
// アイテム生成クラスのヘッダーファイル [title.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _ITEM_GENERATOR_H_		// インクルードガード
#define _ITEM_GENERATOR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "game/item/item.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CField;
class CScore;
class CScreenEffect;

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int ITEM_GENERATOR_PRIORITY = DEFAULT_OBJ_PRIORITY;		// アイテム生成クラスの優先順位

//**********************************************************************************
// *** アイテム生成クラス ***
//**********************************************************************************
class CItemGenerator : public CObject
{
public:
	CItemGenerator(const int nPriority = ITEM_GENERATOR_PRIORITY);
	~CItemGenerator();

	HRESULT Init(void);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Notified(CObject *pObject, std::string_view message, const std::any &args) override;

private:
	CField *m_pField = nullptr;			// 床
	CScore *m_pScore = nullptr;			// スコア表示
	CScreenEffect *m_pScreenEffect = nullptr;			// スクリーンエフェクト
	std::vector<CItem*> m_vpItem;		// 生成したアイテムへのポインタ
	int m_nNumItem = 0;		// アイテムの総数
	CItem::TYPE m_nextType = CItem::TYPE::NONE;		// 次に取るべき食事の種類
	int m_nCombo = 0;		// コンボ数
	int m_nMaxItem = 0;		// 一度に生成するアイテムの最大数
	bool m_bWasCreated = false;		// 既にアイテムが生成されたか
	bool m_bWillSetNext = false;	// 次のアイテムの表示を生成するか
};
#endif