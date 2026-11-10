//==================================================================================
// 
// タイトルシーンのヘッダーファイル [title.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _TITLE_H_		// インクルードガード
#define _TITLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectX;

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CTitle : public CSceneBase<CTitle, CScene::MODE::TITLE>
{
public:
	CTitle();
	~CTitle();

	HRESULT Init(const ManagerContext::SceneContext &sceneContext) override;
	void Uninit(void) override;
	void Update(const ManagerContext::SceneContext &sceneContext) override;
	void Draw(const ManagerContext::SceneContext &sceneContext) override;

private:
	void Start(const ManagerContext::SceneContext &sceneContext);

	CObjectX *m_pCharacter = nullptr;		// タイトルで回転するキャラクター
};
#endif