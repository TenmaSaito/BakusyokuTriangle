//==================================================================================
// 
// 壁クラスのヘッダーファイル [wall.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _WALL_H_		// インクルードガード
#define _WALL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/object3D.h"

//**********************************************************************************
// *** 壁クラス ***
//**********************************************************************************
class CWall : public CObject3D
{
public:
	CWall(const int nPriority = WALL_PRIORITY);
	~CWall();

	static CWall *Create(const Vector3 &pos,
		const Vector3 &rot,
		const Vector2 &size);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &rot,
		const Vector2 &size);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static std::vector<CWall*> FindAll(void);	// シーン上のインスタンスをすべて検索

private:
	static constexpr int WALL_PRIORITY = DEFAULT_OBJ3D_PRIORITY;		// 壁の優先順位
};
#endif