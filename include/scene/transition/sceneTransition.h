//==================================================================================
// 
// 画面遷移クラスのヘッダーファイル [sceneTransition.h]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
#ifndef _SCENE_TRANSITION_H_
#define _SCENE_TRANSITION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/scene.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 画面遷移クラス ***
// 
// INFO : 画面遷移用のクラス
// 遷移の演出がどんなに増えても必ずこの純粋仮想関数のみで使用可能な形にする事
// 
// StartTransition : 遷移演出(フェードイン)開始 
// 演出が画面を完全に隠したときに動作を停止する
// 
// EndTransition : 遷移演出(フェードアウト)開始
// 演出が画面から完全に消えたときに動作を停止する
//**********************************************************************************
class CSceneTransition
{
public:
	// 遷移の種類
	enum class TYPE
	{
		FADE = 0,		// 基本の暗転フェード
		MOUTH,			// 口の食べる動きによる遷移
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const TYPE value) noexcept
	{
		return EnumUtil::InRange(value, TYPE::FADE, TYPE::MAX);
	}

	// 遷移の状態
	enum class STATE
	{
		WIPE_OUT = 0,	// 遷移中 (ワイプアウト)
		WIPE_IN,		// 遷移中 (ワイプイン)
		STAY,			// 待機中
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const STATE value) noexcept
	{
		return EnumUtil::InRange(value, STATE::WIPE_OUT, STATE::MAX);
	}

	CSceneTransition();
	virtual ~CSceneTransition();

	static CSceneTransition *Create(const TYPE type);

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void StartTransition(const int nTransitionTime, const CScene::MODE modeNext) = 0;
	virtual void EndTransition(const int nTransitionTime) = 0;
	STATE GetState(void) const { return m_state; }
	CScene::MODE GetNextMode(void) const { return m_modeNext; }

protected:
	void SetState(const STATE state) { m_state = state; }
	void SetModeNext(CScene::MODE modeNext) { m_modeNext = modeNext; }

private:
	CScene::MODE m_modeNext;		// 次のモード
	STATE m_state;					// 現在の状態
};
#endif