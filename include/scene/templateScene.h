//==================================================================================
// 
// テンプレートシーンのヘッダーファイル [templateScene.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _TEMPLATESCENE_H_		// インクルードガード
#define _TEMPLATESCENE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/scene.h"

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CTemplateScene : public CSceneBase<CTemplateScene, CScene::MODE::NONE>
{
public:
	CTemplateScene();
	~CTemplateScene();

	HRESULT Init(const ManagerContext::SceneContext &sceneContext) override;
	void Uninit(void) override;
	void Update(const ManagerContext::SceneContext &sceneContext) override;
	void Draw(const ManagerContext::SceneContext &sceneContext) override;

private:
	void Start(const ManagerContext::SceneContext &sceneContext);
};
#endif