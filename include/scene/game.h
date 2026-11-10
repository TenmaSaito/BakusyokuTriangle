//==================================================================================
// 
// ゲームシーンのヘッダーファイル [game.h]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
#ifndef _GAME_H_		// インクルードガード
#define _GAME_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/scene.h"

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CGame : public CSceneBase<CGame, CScene::MODE::GAME>
{
public:
	CGame();
	~CGame();

	HRESULT Init(const ManagerContext::SceneContext &sceneContext) override;
	void Uninit(void) override;
	void Update(const ManagerContext::SceneContext &sceneContext) override;
	void Draw(const ManagerContext::SceneContext &sceneContext) override;

private:
	void Start(const ManagerContext::SceneContext &sceneContext);
};
#endif