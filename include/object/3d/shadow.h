//==================================================================================
// 
// 影クラスのヘッダーファイル [shadow.h]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
#ifndef _SHADOW_H_		// インクルードガード
#define _SHADOW_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/object3D.h"

//**********************************************************************************
// *** 影クラス ***
//**********************************************************************************
class CShadow : public CObject3D
{
public:
	CShadow(const int nPriority = SHADOW_PRIORITY);
	~CShadow();

	static CShadow *Create(const Vector3 &pos,
		const Vector3 &rot,
		const Vector2 &size);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &rot,
		const Vector2 &size);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	static constexpr int SHADOW_PRIORITY = DEFAULT_ADD_PRIORITY;		// 影の優先順位
};
#endif