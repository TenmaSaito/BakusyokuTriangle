//==================================================================================
// 
// 床クラスのヘッダーファイル [field.h]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
#ifndef _FIELD_H_
#define _FIELD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/object3D.h"

//**********************************************************************************
// *** 床クラス ***
//**********************************************************************************
class CField : public CObject3D
{
public:
	CField(int nPriority = FIELD_PRIORITY);
	~CField();

	static CField *Create(const Vector3 &pos,
		const Vector3 &rot,
		const Vector2 &size);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &rot,
		const Vector2 &size);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static CField *Find(void);	// シーン上のインスタンスを検索

protected:
	static constexpr int FIELD_PRIORITY = DEFAULT_OBJ3D_PRIORITY;		// 床の優先順位

private:
};
#endif // !_FIELD_H_