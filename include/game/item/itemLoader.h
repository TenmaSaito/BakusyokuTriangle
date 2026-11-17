//==================================================================================
// 
// アイテムスポーン用スクリプト読み込みクラスのヘッダーファイル [itemSpawnerLoader.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _ITEM_SPAWNER_LOADER_H_
#define _ITEM_SPAWNER_LOADER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/script.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CItemSpawner;
class CEatingRule;

//**********************************************************************************
// *** マップスクリプト読み込みクラス ***
//**********************************************************************************
class CItemLoader : public CScript
{
public:
	CItemLoader();
	~CItemLoader();

	HRESULT Load(std::string_view path) override;
	void BindItemSpawner(CItemSpawner *pSpawner) { m_pSpawner = pSpawner; }
	void BindEatingRule(CEatingRule *pRule) { m_pRule = pRule; }

private:
	void LoadSpawner(void);
	void LoadEatingRule(void);

	CItemSpawner *m_pSpawner = nullptr;		// アイテムスポナー
	CEatingRule *m_pRule = nullptr;			// ルール管理
};
#endif