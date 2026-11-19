//==================================================================================
// 
// アイテム関連処理の管理クラスのヘッダーファイル [mealManager.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _MEAL_MANAGER_H_
#define _MEAL_MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CItemSpawner;
class CEatingRule;
class CEatingDirector;
class CItem;
class CPlayer;
class CStage;

//**********************************************************************************
// *** アイテム関連処理の管理クラス ***
//**********************************************************************************
class CMealManager final : public CObject
{
public:
	CMealManager();
	~CMealManager();

	HRESULT Init(std::string_view sPath, 
		CPlayer *pPlayer,
		CStage *pStage);
	void Uninit(void) override;
	void Update(void) override;
	void Notified(CObject *pObject, std::string_view message, const std::any &args) override;

private:
	void SetupDirector(void);
	void SpawnItems(const int nNumItem);
	void DestroyAllItems(void);

	static constexpr int MEAL_MANAGER_PRIORITY = FIRST_PRIORITY;		// このクラスの優先順位
	std::unique_ptr<CItemSpawner> m_pItemSpawner;			// アイテムスポナー
	std::unique_ptr<CEatingRule> m_pEatingRule;				// 取得ルール
	std::unique_ptr<CEatingDirector> m_pEatingDirector;		// 演出管理
	std::vector<CItem*> m_vpItem;		// 生成したアイテム
	CPlayer *m_pPlayer = nullptr;		// プレイヤー
	CStage *m_pStage = nullptr;			// 床
	bool m_bSpawnRequest = false;		// 次のUpdateでアイテムを生成するフラグ
};
#endif