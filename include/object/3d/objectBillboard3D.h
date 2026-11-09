//==================================================================================
// 
// オブジェクトビルボード3Dクラスのヘッダーファイル [objectBillboard3D.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _OBJECTBILLBOARD_3D_H_		// インクルードガード
#define _OBJECTBILLBOARD_3D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include <functional>

//**********************************************************************************
// *** オブジェクトビルボードクラス ***
//**********************************************************************************
class CObjectBillboard3D : public CObject
{
public:
	static CObjectBillboard3D *Create(const Vector3 &pos = VECTOR3_NULL,
		const Vector3 &move = VECTOR3_NULL,
		const Vector2 &size = DEFAULT_OBJBILL_SIZE,
		const int nLife = INT_MAX,
		const int nPriority = DEFAULT_OBJ_PRIORITY);

	CObjectBillboard3D(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CObjectBillboard3D();

	HRESULT Init(const Vector3 &pos, 
		const Vector3 &move,
		const Vector2 &size,
		const int nLife);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetPosition(const Vector3 &position) { m_pos = position; }
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetForce(const Vector3 &force) { m_move = force; }
	const Vector3 *GetForce(void) const { return &m_move; }
	void SetRotation(const Vector3 &rotation);
	const Vector3 *GetRotation(void) const { return &m_rot; }
	void SetSize(const Vector2 &size);
	const Vector2 *GetSize(void) const { return &m_size; }
	void SetColor(const Color &color);
	void SetColor(const std::array<Color, DEFAULT_VERTEX_NUM> &aColor);
	const Color *GetColor(void) const { return &m_col; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void SetAlpha(const bool bAlpha) { m_bAlpha = bAlpha; }
	bool GetAlpha(void) const { return m_bAlpha; }
	void SetStateFunctionBeforeDraw(std::function<void(LPDIRECT3DDEVICE9)> before) { m_beforeDraw = before; }
	void SetStateFunctionAfterDraw(std::function<void(LPDIRECT3DDEVICE9)> after) { m_afterDraw = after; }

protected:
	static constexpr ConstVector2 DEFAULT_OBJBILL_SIZE = ConstVector2(100.0f, 100.0f);		// obj3Dの基本サイズ

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;	// 頂点バッファへのポインタ
	int m_nIdxTexture = -1;		// テクスチャインデックス
	Matrix m_mtxWorld;		// ワールドマトリックス
	const Matrix *m_pMtxParent = nullptr;		// 親ワールドマトリックスへのポインタ
	Vector3 m_pos = VECTOR3_NULL;			// 位置
	Vector3 m_rot = VECTOR3_NULL;			// 角度
	Vector3 m_move = VECTOR3_NULL;			// 移動量
	Vector2 m_size = VECTOR2_NULL;			// サイズ
	Color m_col = COLOR_NULL;			// 色
	float m_fLength = 0.0f;		// 対角線の長さ
	float m_fAngle = 0.0f;			// 対角線の角度
	int m_nLife = 0;			// 体力
	bool m_bDisp = true;			// 描画するか
	bool m_bAlpha = true;			// αテストの有効化
	std::function<void(LPDIRECT3DDEVICE9)> m_beforeDraw;	// Draw関数前の呼び出し関数
	std::function<void(LPDIRECT3DDEVICE9)> m_afterDraw;		// Draw関数後の呼び出し関数
};
#endif