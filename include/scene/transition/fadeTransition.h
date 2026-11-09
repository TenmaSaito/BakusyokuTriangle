//==================================================================================
// 
// 暗転フェードの遷移クラスのヘッダーファイル [fadeTransition.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _FADE_TRANSITION_H_
#define _FADE_TRANSITION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/transition/sceneTransition.h"

//**********************************************************************************
// *** 暗転フェードの遷移クラス ***
//**********************************************************************************
class CFadeTransition : public CSceneTransition
{
public:
	CFadeTransition();
	~CFadeTransition();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void StartTransition(const int nTransitionTime, const CScene::MODE modeNext);
	void EndTransition(const int nTransitionTime);

private:
	void UpdateTransitionIn(void);
	void UpdateTransitionOut(void);
	void UpdateVertex(void);

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;		// 頂点バッファへのポインタ
	int m_nTransitionTime = 0;		// 次の画面へ遷移するまでにかかるフレーム
	float m_fAlpha = 0.0f;			// α値
	float m_fAlphaAdd = 0.0f;		// α値の加算値
};
#endif