//==================================================================================
// 
// タイマークラスのヘッダーファイル [timer.h]
// Author : TENMA SAITO
// Date   : 2026/5/20
// 
//==================================================================================
#ifndef _TIMER_H_
#define _TIMER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CNumber;	

//**********************************************************************************
// *** タイマークラス ***
//**********************************************************************************
class CTimer : public CObject
{
public:
	CTimer(const int nPriority = DEFAULT_TIMER_PRIORITY);
	~CTimer();

	static CTimer *Create(const Vector3 &pos,
		const Vector2 &size,
		const int nNumNumber,
		const int nIdxTexture);

	static CTimer *Create(const Vector3 &pos,
		const Vector2 &size,
		const int nNumNumber,
		const int nScore,
		const int nIdxTexture);

	HRESULT Init(const Vector3 &pos,
		const Vector2 &size,
		const int nNumNumber,
		const int nIdxTexture);
	HRESULT Init(const Vector3 &pos,
		const Vector2 &size,
		const int nNumNumber, 
		const int nScore,
		const int nIdxTexture);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetUpdate(const bool bUpdate);
	bool GetUpdate(void) const;
	void SetTimer(const int nScore);
	void AddTimer(const int nValue);
	int GetTimer(void) const;

	static CTimer *Find(void);	// シーン上のインスタンスを検索

private:
	static constexpr int DEFAULT_TIMER_PRIORITY = DEFAULT_UI_PRIORITY;	// タイマーの基本優先順位

	static constexpr int MAX_TIMERNUM = 8;								// タイマーの最大桁数

	std::array<std::unique_ptr<CNumber>, MAX_TIMERNUM> m_apNumber;		// 数値表示オブジェクトへのポインタ
	int m_nNumTime = 0;		// タイマーの桁数
	int m_nTime = 0;		// 現在のタイマー
	int m_nCounter = 0;		// フレームカウンター
	bool m_bUpdate = true;		// タイマーの自動更新フラグ
};

#endif