//==================================================================================
//
// サービスロケータークラスのヘッダーファイル [serviceLocator.h]
// Author : TENMA SAITO
// Date   : 2026/11/22
//
// アプリの寿命と同じだけ生きるインスタンスを取得する窓口
// ・CManager::Init で生成したものから順に Provide で登録し、CManager::Uninit の最後に Clear する
// ・自分では生成しない (未登録で取得すると assert)
// ・毎フレーム変わるもの (フォーカス中のカメラ等) は登録せず、Update / Draw の引数で受け取る
//
//==================================================================================
#ifndef _SERVICE_LOCATOR_H_		// インクルードガード
#define _SERVICE_LOCATOR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CRenderer;
class CTexture;
class CShader;
class CXFile;
class CLight;
class CSound;
class CDebugProc;
class CInputKeyboard;
class CInputMouse;
class CJoypad;
class CSceneTransition;
class CPause;

//**********************************************************************************
// *** サービスロケータークラス ***
//**********************************************************************************
class CServiceLocator
{
public:
	CServiceLocator() = delete;

	// 登録 (型ごとのオーバーロード)
	static void Provide(const HWND hWnd) { m_hWnd = hWnd; }
	static void Provide(CRenderer *pRenderer) { m_pRenderer = pRenderer; }
	static void Provide(CTexture *pTexture) { m_pTexture = pTexture; }
	static void Provide(CShader *pShader) { m_pShader = pShader; }
	static void Provide(CXFile *pXFile) { m_pXFile = pXFile; }
	static void Provide(CLight *pLight) { m_pLight = pLight; }
	static void Provide(CSound *pSound) { m_pSound = pSound; }
	static void Provide(CDebugProc *pDebugProc) { m_pDebugProc = pDebugProc; }
	static void Provide(CInputKeyboard *pKeyboard) { m_pKeyboard = pKeyboard; }
	static void Provide(CInputMouse *pMouse) { m_pMouse = pMouse; }
	static void Provide(CJoypad *pJoypad) { m_pJoypad = pJoypad; }
	static void Provide(CSceneTransition *pTransition) { m_pTransition = pTransition; }
	static void Provide(CPause *pPause) { m_pPause = pPause; }
	static void Clear(void);

	// 取得
	static HWND GetWindowHandle(void) { return Get(m_hWnd); }
	static CRenderer *GetRenderer(void) { return Get(m_pRenderer); }
	static LPDIRECT3DDEVICE9 GetDevice(void);
	static CTexture *GetTexture(void) { return Get(m_pTexture); }
	static CShader *GetShader(void) { return Get(m_pShader); }
	static CXFile *GetXFile(void) { return Get(m_pXFile); }
	static CLight *GetLight(void) { return Get(m_pLight); }
	static CSound *GetSound(void) { return Get(m_pSound); }
	static CDebugProc *GetDebugProc(void) { return Get(m_pDebugProc); }
	static CInputKeyboard *GetInputKeyboard(void) { return Get(m_pKeyboard); }
	static CInputMouse *GetInputMouse(void) { return Get(m_pMouse); }
	static CJoypad *GetJoypad(void) { return Get(m_pJoypad); }
	static CSceneTransition *GetTransition(void) { return Get(m_pTransition); }
	static CPause *GetPause(void) { return Get(m_pPause); }

private:
	template<class T> static T Get(T pService)
	{ // 未登録 (登録前 / 解除後) の取得はアサーション
		assert(pService != nullptr && "CServiceLocator : 未登録のサービスを取得しようとしました");
		return pService;
	}

	inline static HWND m_hWnd = nullptr;						// ウィンドウハンドル
	inline static CRenderer *m_pRenderer = nullptr;				// レンダラー
	inline static CTexture *m_pTexture = nullptr;				// テクスチャ管理
	inline static CShader *m_pShader = nullptr;					// シェーダー管理
	inline static CXFile *m_pXFile = nullptr;					// XFile管理
	inline static CLight *m_pLight = nullptr;					// ライト
	inline static CSound *m_pSound = nullptr;					// サウンド
	inline static CDebugProc *m_pDebugProc = nullptr;			// デバッグ表示
	inline static CInputKeyboard *m_pKeyboard = nullptr;		// キーボード
	inline static CInputMouse *m_pMouse = nullptr;				// マウス
	inline static CJoypad *m_pJoypad = nullptr;					// ジョイパッド
	inline static CSceneTransition *m_pTransition = nullptr;	// 遷移演出
	inline static CPause *m_pPause = nullptr;					// ポーズ
};
#endif
