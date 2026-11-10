//==================================================================================
// 
// 画面下に表示されるUIクラスのヘッダーファイル [nextInfo.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _NEXTINFO_H_
#define _NEXTINFO_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObject2D;

//**********************************************************************************
// *** 画面下に表示されるUIクラス ***
//**********************************************************************************
class CNextInfo : public CObject
{
public:
	CNextInfo(const int nPriority = NEXTINFO_PRIORITY);
	~CNextInfo();

	static CNextInfo *Create(void);

	HRESULT Init(void);
	void Uninit(void) override;
	void Update(void) override;
	void SetNext(CObject2D *pNext);
	void Angry(const int nAngryFrame = ANGRY_FRAME);

	static CNextInfo *Find(void);	// シーン上のインスタンスを検索

private:
	static constexpr int NEXTINFO_PRIORITY = DEFAULT_2DUI_PRIORITY;		// アイテムUI表示の優先順位

	static constexpr int ANGRY_FRAME = 90;		// 怒るフレーム数
	CObject2D *m_pBase = nullptr;		// 一番下に表示されるコメント
	CObject2D *m_pNext = nullptr;		// 次に取得すべきアイテム
	bool m_bIsAngry = true;		// 次に取得すべきアイテムを表示するか
	int m_nAngryCount = 0;		// 怒ったフレーム数
	int m_nCountWaitAlphaDecrease = 0;	// α値減少待機フレーム数
	float m_fAlpha = 0.0f;		// α値
};
#endif