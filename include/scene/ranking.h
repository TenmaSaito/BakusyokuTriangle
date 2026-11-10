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

	HRESULT Init(const ManagerContext::SceneContext &sceneContext) override;
	void Uninit(void) override;
	void Update(const ManagerContext::SceneContext &sceneContext) override;
	void Draw(const ManagerContext::SceneContext &sceneContext) override;

private:
	void Start(const ManagerContext::SceneContext &sceneContext);
	void CreateRanking(const ManagerContext::SceneContext &sceneContext);
};
#endif