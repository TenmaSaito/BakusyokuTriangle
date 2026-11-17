//==================================================================================
// 
// アイテム取得時演出の管理クラスのヘッダーファイル [item.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _EATING_DIRECTOR_H_
#define _EATING_DIRECTOR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/eatingRule.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CSound;
class CScore;
class CCamera;
class CNextInfo;
class CScreenEffect;

//**********************************************************************************
// *** アイテム取得時演出の管理クラス ***
//**********************************************************************************
class CEatingDirector final
{
public:
	CEatingDirector();
	~CEatingDirector();

	// 咀嚼演出はitemがある
	// こいつが行うのは、音声の再生やスコア加算の管理
	// 画面全体に影響する演出の再生
	void Play(const CEatingRule::EAT_RESULT result,
		const CItem::TYPE next, 
		const int nScore);
	void BindSound(CSound *pSound) { m_pSound = pSound; }
	void BindScore(CScore *pScore) { m_pScore = pScore; }
	void BindScreenEffect(CScreenEffect *pScreenEffect) { m_pScreenEffect = pScreenEffect; }
	void BindCameras(CCamera *pCamPlayer, CCamera *pCamField) { m_pCamPlayer = pCamPlayer; m_pCamField = pCamField; }

private:
	void Success(const int nScore);
	void Fail(void);
	void WaveClear(const int nScore);
	void SetDirection(const int nSoundType, const Color col);

	CScreenEffect *m_pScreenEffect = nullptr;		// 画面演出
	CNextInfo *m_pNextInfo = nullptr;		// 次に取得するアイテムを表示するUI
	CCamera *m_pCamPlayer = nullptr;	// プレイヤーカメラ
	CCamera *m_pCamField = nullptr;		// フィールドカメラ
	CSound *m_pSound = nullptr;		// サウンド
	CScore *m_pScore = nullptr;		// スコアクラス
};
#endif