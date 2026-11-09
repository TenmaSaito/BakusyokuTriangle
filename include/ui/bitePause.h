//==================================================================================
//
// 牙演出付きポーズクラスのヘッダーファイル [bitePause.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _BITE_PAUSE_H_		// インクルードガード
#define _BITE_PAUSE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "ui/pause.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** 牙演出付きポーズクラス ***
//**********************************************************************************
class CBitePause : public CPause
{
public:
	CBitePause();
	~CBitePause() override;

	static CBitePause *Create(void);

	HRESULT Init(void);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	void ActionType(void) override;
	void UpdateJaw(void);

	std::unique_ptr<CPolygon2D> m_pUpper;		// 上の牙
	std::unique_ptr<CPolygon2D> m_pLower;		// 下の牙
	TYPE m_selectedType = TYPE::CONTINUE;		// 決定時に選ばれていたタイプ
	int m_nBiteFrame = 0;		// 噛みつきの経過フレーム数
	bool m_bBiting = false;		// 噛みつき中フラグ
};
#endif
