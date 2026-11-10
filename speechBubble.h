//==================================================================================
// 
// 吹き出しクラスのヘッダーファイル [speechBubble.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _SPEECH_BUBBLE_H_		// インクルードガード
#define _SPEECH_BUBBLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/objectBillboard3D.h"

//**********************************************************************************
// *** 吹き出しクラス ***
//**********************************************************************************
class CSpeechBubble : public CObjectBillboard3D
{
public:
	CSpeechBubble(const int nPriority = DEFAULT_3DUI_PRIORITY);
	~CSpeechBubble();

	HRESULT Init(const Vector3 &offset,
		const Vector2 &size);
	void Uninit(void) override;
	void Update(void) override;
};
#endif