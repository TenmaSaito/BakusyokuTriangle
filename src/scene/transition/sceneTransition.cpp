//==================================================================================
// 
// 画面遷移クラスのソースファイル [sceneTransition.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/transition/sceneTransition.h"
#include "scene/transition/fadeTransition.h"
#include "scene/transition/mouthTransition.h"

//==================================================================================
// --- トランジションの作成 ---
//==================================================================================
CSceneTransition *CSceneTransition::Create(const TYPE type)
{
	CSceneTransition *pTransition = nullptr;		// 生成したトランジションへのポインタ

	switch (type)
	{
	case TYPE::FADE:				// 暗転フェード
		pTransition = new CFadeTransition;
		break;

	case TYPE::MOUTH:			// 口に食べられる遷移
		pTransition = new CMouthTransition;
		break;

	default:
		break;
	}

	if (pTransition)
	{ // 生成に成功していれば、初期化
		pTransition->Init();
	}

	return pTransition;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSceneTransition::CSceneTransition()
{ // メンバ変数の初期化
	m_modeNext = CScene::MODE::NONE;
	m_state = STATE::STAY;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSceneTransition::~CSceneTransition()
{
}