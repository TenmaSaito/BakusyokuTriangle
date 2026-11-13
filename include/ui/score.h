//==================================================================================
// 
// スコアクラスのヘッダーファイル [score.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _SCORE_H_
#define _SCORE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CNumber;		// 数値表示クラス

//**********************************************************************************
// *** スコアクラス ***
//**********************************************************************************
class CScore : public CObject
{
public:
	CScore(const int nPriority = SCORE_PRIORITY);
	~CScore();

	static CScore *Create(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber);
	static CScore *Create(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber, const int nScore);

	HRESULT Init(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber);
	HRESULT Init(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber, const int nScore);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetScore(const int nScore);
	void AddScore(const int nValue);
	int GetScore(void) const;

	static CScore *Find(void);	// シーン上のインスタンスを検索

private:
	static constexpr int SCORE_PRIORITY = DEFAULT_2DUI_PRIORITY;		// スコアの基本優先順位

	static constexpr int MAX_SCORENUM = 8;							// スコアの最大桁数

	CNumber *m_apNumber[MAX_SCORENUM] = {};			// 数値表示オブジェクトへのポインタ
	int m_nNumScore = 0;	// スコアの桁数
	int m_nScore = 0;		// 現在のスコア
	int m_nAdditionalScore = 0;		// 追加予定のスコア
};

#endif