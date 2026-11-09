//==================================================================================
// 
// ランキングシーンのヘッダーファイル [ranking.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _RANKING_H_		// インクルードガード
#define _RANKING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/scene.h"

//**********************************************************************************
// *** ランキングクラス ***
//**********************************************************************************
class CRanking : public CSceneBase<CRanking, CScene::MODE::RANKING>
{
public:
	CRanking();
	~CRanking();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	void Start(void);
	void CreateRanking(void);
};
#endif