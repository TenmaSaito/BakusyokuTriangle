//==================================================================================
//
// 口による画面遷移クラスのヘッダーファイル [mouthTransition.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _MOUTH_TRANSITION_H_
#define _MOUTH_TRANSITION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/transition/sceneTransition.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** 口による画面遷移クラス ***
//**********************************************************************************
class CMouthTransition final : public CSceneTransition
{
public:
    CMouthTransition();
    ~CMouthTransition() override;

    HRESULT Init(void) override;
    void Uninit(void) override;
    void Update(void) override;
    void Draw(void) override;
    void StartTransition(int nTransitionTime, CScene::MODE modeNext) override;
    void EndTransition(int nTransitionTime) override;

private:
	// 遷移の進行度 (0.0～1.0) から口の位置と角度を設定
    void ApplyPose(float progress);

    std::unique_ptr<CPolygon2D> m_pUpper;	// 上側の口
    std::unique_ptr<CPolygon2D> m_pLower;	// 下側の口
    int m_duration = 1;				// 片道の遷移時間
    int m_elapsed = 0;				// 経過フレーム数
};
#endif