//==================================================================================
//
// サービスロケータークラスのソースファイル [serviceLocator.cpp]
// Author : TENMA SAITO
// Date   : 2026/11/22
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/serviceLocator.h"
#include "render/renderer.h"

//==================================================================================
// --- デバイスの取得処理 ---
//==================================================================================
LPDIRECT3DDEVICE9 CServiceLocator::GetDevice(void)
{
	return Get(GetRenderer()->GetDevice());
}

//==================================================================================
// --- 全登録の解除処理 ---
//==================================================================================
void CServiceLocator::Clear(void)
{
	m_hWnd = nullptr;
	m_pRenderer = nullptr;
	m_pTexture = nullptr;
	m_pXFile = nullptr;
	m_pLight = nullptr;
	m_pSound = nullptr;
	m_pDebugProc = nullptr;
	m_pKeyboard = nullptr;
	m_pMouse = nullptr;
	m_pJoypad = nullptr;
	m_pTransition = nullptr;
	m_pPause = nullptr;
}
