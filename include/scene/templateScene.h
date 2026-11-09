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

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	void Start(void);
};
#endif