//==================================================================================
// 
// マネージャクラスのヘッダーファイル [manager.h]
// Author : TENMA SAITO
// Date   : 2026/5/12
// 
//==================================================================================
#ifndef _MANAGER_H_		// インクルードガード
#define _MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "scene/scene.h"
#define ENABLE_INPUT_SYSTEM
#ifdef ENABLE_INPUT_SYSTEM
#include "input/inputSystem.h"
#endif

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define ENABLE_INPUT_SYSTEM

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int DEFAULT_APPLICATION_UPDATE_PER_SECOND = 60;		// 一秒間にUpdateする回数
constexpr int DEFAULT_APPLICATION_DRAW_PER_SECOND = 60;			// 一秒間にDrawする回数

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CRenderer;
#ifndef ENABLE_INPUT_SYSTEM
class CInputKeyboard;
class CInputMouse;	
class CJoypad;
#else
#endif
class CDebugProc;	
class CSound;		
class CLight;
class CScene;
class CSceneTransition;
class CPause;

//**********************************************************************************
// *** マネージャクラス ***
//**********************************************************************************
class CManager
{
public:
	HRESULT Init(const HINSTANCE hInstance,
		const HWND hWnd,
		const BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetFPS(const int nFPS) { m_nCountFPS = nFPS; }
	int GetFPS(void) { return m_nCountFPS; }
	void SetEnablePause(const bool bEnable);
	bool GetEnablePause(void);
	CRenderer *GetRenderer(void) const { return m_pRenderer.get(); }
#ifndef ENABLE_INPUT_SYSTEM
	CInputKeyboard *GetInputKeyboard(void) const { return m_pInputKeyboard.get(); }
	CInputMouse *GetInputMouse(void) const { return m_pInputMouse.get(); }
	CJoypad *GetJoypad(void) const { return m_pJoypad.get(); }
#else
	CInputKeyboard *GetInputKeyboard(void) const { return CInputSystem::GetInstance()->GetKeyboard(); }
	CInputMouse *GetInputMouse(void) const { return CInputSystem::GetInstance()->GetMouse(); }
	CJoypad *GetJoypad(void) const { return CInputSystem::GetInstance()->GetJoypad(); }
#endif
	CDebugProc *GetDebugProc(void) const { return m_pDebugProc.get(); }
	CSound *GetSound(void) const { return m_pSound.get(); }
	CLight *GetLight(void) const { return m_pLight.get(); }
	CSceneTransition *GetTransition(void) const { return m_pTransition.get(); }
	CPause *GetPause(void) const { return m_pPause.get(); }
	HWND GetWindowHandle(void) const { return m_hWnd; }
	HRESULT SetMode(const CScene::MODE modeNext);
	CScene::MODE GetMode(void) const { return m_pScene->GetMode(); }
	void SetTransition(const CScene::MODE modeNext);
	bool IsTransition(void) const;
	template<IsScene T> T *GetScene(void) const;

	static CManager *GetInstance(void);
	static constexpr int SecToFrame(const int nSeconds) { return nSeconds * DEFAULT_APPLICATION_UPDATE_PER_SECOND; }
	static constexpr float SecToFrame(const float fSeconds) { return fSeconds * static_cast<float>(DEFAULT_APPLICATION_UPDATE_PER_SECOND); }
	static constexpr int FrameToSec(const int nFrame) { return nFrame / DEFAULT_APPLICATION_UPDATE_PER_SECOND; }
	static constexpr float FrameToSec(const float fFrame) { return fFrame / static_cast<float>(DEFAULT_APPLICATION_UPDATE_PER_SECOND); }
	static constexpr float RatioToSec(const float fT) { return FrameToSec(1.0f / fT); }
	static constexpr float SecToRatio(const float fSec) { return 1.0f / SecToFrame(fSec); }
	static constexpr float FrameToRatio(const float fFrame) { return 1.0f / fFrame; }
	static constexpr float RatioToFrame(const float fT) { return 1.0f / fT ; }

private:
	CManager();
	~CManager();

	std::unique_ptr<CRenderer> m_pRenderer;				// レンダラーへのポインタ
#ifndef ENABLE_INPUT_SYSTEM
	std::unique_ptr<CInputKeyboard> m_pInputKeyboard;	// キーボードオブジェクトへのポインタ
	std::unique_ptr<CInputMouse> m_pInputMouse;			// マウスオブジェクトへのポインタ
	std::unique_ptr<CJoypad> m_pJoypad;					// ジョイパッドオブジェクトへのポインタ
#else

#endif
	std::unique_ptr<CDebugProc> m_pDebugProc;			// デバッグ表示オブジェクトへのポインタ
	std::unique_ptr<CSound> m_pSound;					// サウンドオブジェクトへのポインタ
	std::unique_ptr<CLight> m_pLight;					// ライトオブジェクトへのポインタ
	std::unique_ptr<CScene> m_pScene;					// シーンへのポインタ
	std::unique_ptr<CSceneTransition> m_pTransition;	// 遷移演出へのポインタ
	std::unique_ptr<CPause> m_pPause;					// ポーズへのポインタ
	HWND m_hWnd = nullptr;		// ウィンドウハンドル
	int m_nCountFPS = 0;		// FPSカウント
	int m_nCounterFrame = 0;	// フレームカウンター
	int m_nCountReload = 0;		// リロードカウント
	bool m_bPause = false;		// ポーズ状態
};

//==================================================================================
// --- キャスト後シーン取得処理 ---
//==================================================================================
template<IsScene T> T *CManager::GetScene(void) const
{
	if (m_pScene->GetMode() != T::GetOwnMode())
	{ // 取得しようとしたモードと現在のモードが異なる場合
		return nullptr;
	}

	// シーンのポインタをキャスト
	return static_cast<T*>(m_pScene.get());
}
#endif