//==================================================================================
// 
// オブジェクトXクラスのヘッダーファイル [objectX.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _OBJECTX_H_		// インクルードガード
#define _OBJECTX_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "object/base/parentable.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int DEFAULT_OBJX_PRIORITY = DEFAULT_OBJ_PRIORITY;		// objXの基本優先順位	
inline constexpr ConstVector3 DEFAULT_OBJX_POS = VECTOR3_NULL;			// objXの基本位置
inline constexpr ConstVector3 DEFAULT_OBJX_ROT = VECTOR3_NULL;			// objXの基本角度

//**********************************************************************************
// *** オブジェクトXクラス ***
//**********************************************************************************
class CObjectX : public CObject, public IParentable
{
public:
	CObjectX(const int nPriority = DEFAULT_OBJX_PRIORITY);
	~CObjectX();

	static CObjectX *Create(std::string_view pFilename,
		const Vector3 &pos = DEFAULT_OBJX_POS,
		const Vector3 &rot = DEFAULT_OBJX_ROT);

	HRESULT Init(std::string_view pFilename,
		const Vector3 &pos,
		const Vector3 &rot);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Draw(const Matrix &mtx);
	void DrawShadow(const Matrix &mtxShadow);
	void SetPosition(const Vector3 &position) { m_pos = position; }
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation) { m_rot = rotation; }
	const Vector3 *GetRotation(void) const { return &m_rot; }
	const Matrix *GetMatrix(void);
	const Vector3 *GetVtxMin(void) const { return &m_vtxMin; }
	const Vector3 *GetVtxMax(void) const { return &m_vtxMax; }
	std::string_view GetFileName(void) const { return m_sFilename; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return m_pMtxParent; }
	bool IsHitByRay(const Vector3 &start, const Vector3 &vec, const float fLength);

protected:
	Matrix *GetMatrixPtr(void) { return &m_mtxWorld; }

private:
	HRESULT	LoadXFile(std::string_view pFilename);

	LPD3DXMESH m_pMesh = nullptr;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat = nullptr;		// マテリアルへのポインタ
	std::vector<int> m_vIdx;				// テクスチャインデックスの配列
	DWORD m_dwNumMat = 0U;					// マテリアルの数
	Vector3 m_vtxMin = VECTOR3_NULL;		// モデルの各最小頂点の位置
	Vector3	m_vtxMax = VECTOR3_NULL;		// モデルの各最大頂点の位置
	Matrix m_mtxWorld;						// ワールドマトリックス
	const Matrix *m_pMtxParent = nullptr;	// 親マトリックスへのポインタ
	Vector3 m_pos = VECTOR3_NULL;			// 位置
	Vector3 m_rot = VECTOR3_NULL;			// 角度
	std::string m_sFilename;				// ファイル名
	bool m_bMtxInitialize = false;			// マトリックスの計算フラグ
};
#endif