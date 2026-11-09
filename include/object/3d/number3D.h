//==================================================================================
// 
// 数字の3D表示(ポリゴン)クラスのヘッダーファイル [number3D.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _NUMBER_3D_H_		// インクルードガード
#define _NUMBER_3D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "object/base/parentable.h"

//**********************************************************************************
// *** 数値3D表示クラス ***
//**********************************************************************************
class CNumber3D : public IParentable
{
public:
	CNumber3D();
	virtual ~CNumber3D();

	static CNumber3D *Create(const Vector3 &pos,
		const Vector2 &size,
		const int nValue);

	HRESULT Init(const Vector3 &pos,
		const Vector2 &size,
		const int nValue);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void Draw(const Matrix &mtx);
	void SetPosition(const Vector3 &position) { m_pos = position; }
	const Vector3 &GetPosition(void) const { return m_pos; }
	void SetRotation(const Vector3 &rotation) { m_rot = rotation; }
	const Vector3 &GetRotation(void) const { return m_rot; }
	void SetSize(const Vector2 &size);
	const Vector2 &GetSize(void) const { return m_size; }
	void SetColor(const Color &col);
	void SetAlpha(const float fAlpha) { SetColor(Color(m_col.r, m_col.g, m_col.b, fAlpha)); }
	const Color &GetColor(void) const { return m_col; }
	void SetNumber(const int nNumber);
	int GetNumber(void) const { return m_nNumber; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	const Matrix *GetMatrix(void) const { return &m_mtxWorld; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return m_pMtxParent; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;			// 頂点バッファ
	const Matrix *m_pMtxParent = nullptr;		// 親マトリックスへのポインタ
	Matrix m_mtxWorld;					// ワールドマトリックス
	Vector3 m_pos = VECTOR3_NULL;		// オブジェクトの位置
	Vector3 m_rot = VECTOR3_NULL;		// オブジェクトの角度
	Vector2 m_size = VECTOR2_NULL;		// オブジェクトのサイズ
	Color m_col = COLOR_NULL;	// 色
	int m_nNumber = 0;			// 値
	int m_nIdxTexture = -1;		// テクスチャインデックス
	bool m_bDisp = true;		// 描画フラグ
};
#endif
