//==================================================================================
// 
// オブジェクト3Dクラスのヘッダーファイル [object3D.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _OBJECT3D_H_		// インクルードガード
#define _OBJECT3D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "object/base/parentable.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int DEFAULT_OBJ3D_PRIORITY = DEFAULT_OBJ_PRIORITY;	// obj3Dの基本優先順位	
constexpr bool DEFAULT_OBJ3D_FLAG = false;						// obj3Dの基本フラグ
inline constexpr ConstVector3 DEFAULT_OBJ3D_POS = VECTOR3_NULL;				// obj3Dの基本位置
inline constexpr ConstVector3 DEFAULT_OBJ3D_ROT = VECTOR3_NULL;				// obj3Dの基本角度
inline constexpr ConstVector2 DEFAULT_OBJ3D_SIZE = ConstVector2(100.0f, 100.0f);	// obj3Dの基本サイズ

//**********************************************************************************
// *** オブジェクト3Dクラス ***
//**********************************************************************************
class CObject3D : public CObject, public IParentable
{
public:
	CObject3D(const int nPriority = DEFAULT_OBJ3D_PRIORITY);
	~CObject3D();

	static CObject3D *Create(const Vector3 &pos = DEFAULT_OBJ3D_POS,
		const Vector3 &rot = DEFAULT_OBJ3D_ROT,
		const Vector2 &size = DEFAULT_OBJ3D_SIZE,
		const bool bXYPlane = DEFAULT_OBJ3D_FLAG);

	HRESULT Init(const Vector3 &pos, 
		const Vector3 &rot, 
		const Vector2 &size,
		const bool bXYPlane);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetPosition(const Vector3 &position);
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation);
	const Vector3 *GetRotation(void) const { return &m_rot; }
	void SetSize(const Vector2 &size);
	const Vector2 *GetSize(void) const { return &m_size; }
	const Matrix *GetMatrix(void);
	void SetColor(const Color &col);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return &m_mtxWorld; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;	// 頂点バッファへのポインタ
	int m_nIdxTexture = -1;					// テクスチャのインデックス
	Matrix m_mtxWorld;						// ワールドマトリックス
	const Matrix *m_pMtxParent = nullptr;	// 親マトリックスへのポインタ
	Vector3 m_pos = VECTOR3_NULL;			// 位置
	Vector3 m_rot = VECTOR3_NULL;			// 角度
	Vector2 m_size = VECTOR2_NULL;			// サイズ
	bool m_bXYPlane = false;				// XYポリゴンのフラグ
	bool m_bMtxInitialize = false;			// マトリックスの計算フラグ
};
#endif