//==================================================================================
//
// シーンマネージャクラスのソースファイル [sceneManager.cpp]
// Author : TENMA SAITO
// Date   : 2026/11/22
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/sceneManager.h"
#include "core/serviceLocator.h"
#include "render/renderer.h"
#include "render/camera.h"
#include "render/texture.h"
#include "render/shader.h"
#include "render/xfile.h"
#include "object/base/object.h"
#include "input/inputSystem.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSceneManager::CSceneManager()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSceneManager::~CSceneManager()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CSceneManager::Init(CRenderer *pRenderer,
	CLight *pLight,
	CSound *pSound,
	CSceneTransition *pTransition,
	const int nTransitionFrame)
{
	m_pRenderer = pRenderer;
	m_pLight = pLight;
	m_pSound = pSound;
	m_pTransition = pTransition;
	m_nTransitionFrame = nTransitionFrame;

	return (m_pRenderer == nullptr) ? E_FAIL : S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CSceneManager::Uninit(void)
{
	if (m_pScene != nullptr)
	{ // シーンが生成されていれば終了処理
		m_pScene->Uninit();
		m_pScene.reset();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CSceneManager::Update(void)
{
	if (m_pScene == nullptr) return;

	// シーンの更新
	m_pScene->Update(CreateSceneContext());
}

//==================================================================================
// --- シーン変更処理 ---
//==================================================================================
HRESULT CSceneManager::ChangeScene(const CScene::MODE modeNext)
{
	// 現在のシーンの終了
	Uninit();

	// 全オブジェクトの破棄
	CObject::ReleaseAll();

	// カメラの破棄
	CCamera::ReleaseAll();

	// シーンの生成
	CScene::Create(CreateSceneContext(), modeNext, m_pScene);
	return (m_pScene == nullptr) ? E_FAIL : S_OK;
}

//==================================================================================
// --- 描画コンテキストの作成処理 ---
//==================================================================================
ManagerContext::RenderContext CSceneManager::CreateRenderContext(void) const
{
	ManagerContext::RenderContext renderContext;		// 描画コンテキスト

	renderContext.pDevice = m_pRenderer->GetDevice();
	renderContext.pFocus = CCamera::GetCamera(CCamera::GetFocus());
	renderContext.pLight = m_pLight;
	renderContext.pTexture = CServiceLocator::GetTexture();
	renderContext.pShader = CServiceLocator::GetShader();
	renderContext.pXFile = CServiceLocator::GetXFile();
	renderContext.pDebugProc = CServiceLocator::GetDebugProc();

	return renderContext;
}

//==================================================================================
// --- シーンコンテキストの作成処理 ---
//==================================================================================
ManagerContext::SceneContext CSceneManager::CreateSceneContext(void)
{
	// メンバの描画コンテキストを最新にする (シーンが保持してもポインタは無効にならない)
	m_renderContext = CreateRenderContext();

	return ManagerContext::SceneContext
	{
		.pRenderContext = &m_renderContext,
		.pInputSystem = CInputSystem::GetInstance(),
		.pSound = m_pSound,
		.pTransition = m_pTransition,
		.TRANSITION_FRAME = m_nTransitionFrame,
	};
}
