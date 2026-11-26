//==================================================================================
//
// マネージャから各所へ渡すコンテキストのヘッダーファイル [managerContext.h]
// Author : TENMA SAITO
// Date   : 2026/11/22
//
//==================================================================================
#ifndef _MANAGER_CONTEXT_H_		// インクルードガード
#define _MANAGER_CONTEXT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CCamera;
class CLight;
class CTexture;
class CXFile;
class CInputSystem;
class CSound;
class CSceneTransition;

namespace ManagerContext
{
	// 各オブジェクトの初期化、描画時に必要な各インスタンスへのポインタ
	struct RenderContext
	{
		LPDIRECT3DDEVICE9 pDevice = nullptr;		// デバイス
		CCamera *pFocus = nullptr;					// フォーカス中のカメラ
		CLight *pLight = nullptr;					// ライト
		CTexture *pTexture = nullptr;				// テクスチャ管理
		CXFile *pXFile = nullptr;					// XFile管理
	};

	// 各シーンに渡される、必要な各インスタンスへのポインタ
	struct SceneContext
	{
		RenderContext *pRenderContext = nullptr;	// 描画に必要なコンテキスト
		CInputSystem *pInputSystem = nullptr;		// インプットシステム
		CSound *pSound = nullptr;					// サウンド
		CSceneTransition *pTransition = nullptr;	// 遷移演出
		const int TRANSITION_FRAME = 0;				// 遷移にかかる時間
	};
}
#endif
