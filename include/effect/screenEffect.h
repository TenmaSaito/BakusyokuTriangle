//==================================================================================
// 
// スクリーンエフェクトクラスのヘッダーファイル [screenEffect.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _SCREEN_EFFECT_H_		// インクルードガード
#define _SCREEN_EFFECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/2d/object2D.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int SCREEN_EFFECT_PRIORITY = FINALLY_PRIORITY;		// スクリーンエフェクトの描画順位

//**********************************************************************************
// *** スクリーンエフェクトクラス ***
//**********************************************************************************
class CScreenEffect : public CObject2D
{
public:
	CScreenEffect(const int nPriority = SCREEN_EFFECT_PRIORITY);
	~CScreenEffect();

	static CScreenEffect *Create(const Color &color,
		const float fFlashFrame);

	HRESULT Init(const Color &color,
		const float fFlashFrame);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Flash(const float fAlpha = DEFAULT_FLASH_ALPHA);

private:
	static constexpr float DEFAULT_FLASH_ALPHA = 1.0f;		// デフォルトのフラッシュ光度
	float m_fAlphaDecrease = 0.0f;
};
#endif