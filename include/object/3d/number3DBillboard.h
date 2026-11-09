//==================================================================================
// 
// 数字の3D表示(ビルボード)クラスのヘッダーファイル [number3DBillboard.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _NUMBER_3D_BILLBOARD_H_		// インクルードガード
#define _NUMBER_3D_BILLBOARD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/number3D.h"

//**********************************************************************************
// *** 数値3D表示クラス ***
//**********************************************************************************
class CNumber3DBillboard : public CNumber3D
{
public:
	CNumber3DBillboard();
	~CNumber3DBillboard();

	static CNumber3DBillboard *Create(const Vector3 &pos,
		const Vector2 &size,
		const int nValue);

	HRESULT Init(const Vector3 &pos,
		const Vector2 &size,
		const int nValue);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
};
#endif
