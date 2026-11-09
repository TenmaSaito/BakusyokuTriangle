//==================================================================================
// 
// モデルクラスのヘッダーファイル [model.h]
// Author : TENMA SAITO
// Date   : 2026/6/2
// 
//==================================================================================
#ifndef _MODEL_H_		// インクルードガード
#define _MODEL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "object/base/parentable.h"

//**********************************************************************************
// *** モデルクラス ***
//**********************************************************************************
class CModel : public IParentable
{
public:
	CModel();
	~CModel();

	static CModel *Create(const char *pXFileName,
		const Vector3 &pos,
		const Vector3 &rot);

	HRESULT Init(const char *pXFileName,
		const Vector3 &pos,
		const Vector3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return m_pMtxParent; }
	const Matrix *GetMtxWorld(void) const { return &m_mtxWorld; }
	void SetPosition(const Vector3 &position) { m_pos = position; }
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation) { m_rot = rotation; }
	const Vector3 *GetRotation(void) const { return &m_rot; }
	std::string_view GetFileName(void) const { return m_sFileName; }

private:
	LPD3DXMESH m_pMesh = nullptr;		// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat = nullptr;	// マテリアルへのポインタ
	std::string m_sFileName;			// ファイル名
	std::vector<int> m_vIdx;			// テクスチャインデックスの配列
	DWORD m_dwNumMat = 0U;				// マテリアルの数
	Vector3 m_pos = VECTOR3_NULL;		// 位置
	Vector3 m_rot = VECTOR3_NULL;		// 角度
	Matrix m_mtxWorld;					// ワールドマトリックス
	const Matrix *m_pMtxParent = nullptr;		// 親マトリックスへのポインタ
};
#endif