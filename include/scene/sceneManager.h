//==================================================================================
//
// シーンマネージャクラスのヘッダーファイル [sceneManager.h]
// Author : TENMA SAITO
// Date   : 2026/11/22
//
//==================================================================================
#ifndef _SCENE_MANAGER_H_		// インクルードガード
#define _SCENE_MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "core/managerContext.h"
#include "scene/scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CRenderer;
class CLight;
class CSound;
class CSceneTransition;

//**********************************************************************************
// *** シーンマネージャクラス ***
// 現在のシーンを所有し、描画コンテキストを作って配る
//**********************************************************************************
class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();

	HRESULT Init(CRenderer *pRenderer,
		CLight *pLight,
		CSound *pSound,
		CSceneTransition *pTransition,
		const int nTransitionFrame);
	void Uninit(void);
	void Update(void);
	HRESULT ChangeScene(const CScene::MODE modeNext);
	CScene::MODE GetMode(void) const { return m_pScene->GetMode(); }
	template<IsScene T> T *GetScene(void) const;
	ManagerContext::RenderContext CreateRenderContext(void) const;

private:
	ManagerContext::SceneContext CreateSceneContext(void);

	std::unique_ptr<CScene> m_pScene;					// 現在のシーン
	ManagerContext::RenderContext m_renderContext;		// シーンに渡す描画コンテキスト (使う直前に更新)
	CRenderer *m_pRenderer = nullptr;					// レンダラー
	CLight *m_pLight = nullptr;							// ライト
	CSound *m_pSound = nullptr;							// サウンド
	CSceneTransition *m_pTransition = nullptr;			// 遷移演出
	int m_nTransitionFrame = 0;							// 遷移にかかる時間
};

//==================================================================================
// --- キャスト後シーン取得処理 ---
//==================================================================================
template<IsScene T> T *CSceneManager::GetScene(void) const
{
	if (m_pScene == nullptr || m_pScene->GetMode() != T::GetOwnMode())
	{ // 取得しようとしたモードと現在のモードが異なる場合
		return nullptr;
	}

	// シーンのポインタをキャスト
	return static_cast<T*>(m_pScene.get());
}
#endif
