//==================================================================================
// 
// メッシュリングクラスのヘッダーファイル [meshRing.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _MESH_RING_H_
#define _MESH_RING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "core/resourceable.h"
#include "object/base/parentable.h"

//**********************************************************************************
// *** メッシュリングクラス ***
//**********************************************************************************
class CMeshRing : public CObject, public IParentable
{
public:
	CMeshRing(const int nPriority = MESH_RING_PRIORITY);
	~CMeshRing();

	static CMeshRing *Create(const Vector3 &pos,
		const Vector3 &rot,
		const float fRadiusInner,
		const float fRingWidth,
		const int nNumBlockRing);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &rot,
		const float fRadiusInner,
		const float fRingWidth,
		const int nNumBlockRing);
	void Uninit(void) override;
	void Update(const ManagerContext::RenderContext &context) override;
	void Draw(const ManagerContext::RenderContext &context) override;
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetParent(const Matrix *pMtxParent = nullptr) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return m_pMtxParent; }

protected:
	void SetPosition(const Vector3 &pos) { m_pos = pos; }
	void SetRotation(const Vector3 &rot) { m_rot = rot; }
	void SetRadiusInner(const float fRadiusInner);
	void SetRingWidth(const float fRingWidth);

private:
	HRESULT CreateVertex(const LPDIRECT3DDEVICE9 pDevice);
	HRESULT CreateIndex(const LPDIRECT3DDEVICE9 pDevice);
	void UpdateVertexPosition(void);

	static constexpr int MESH_RING_PRIORITY = DEFAULT_OBJ_PRIORITY;		// メッシュリングの優先順位
	int m_nIdxTexture = IResourceable::INVALID_RESOURCE_ID;		// テクスチャインデックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;	// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff = nullptr;	// インデックスバッファ
	const Matrix *m_pMtxParent = nullptr;			// 親マトリックスへのポインタ
	Vector3 m_pos = VECTOR3_NULL;		// 位置
	Vector3 m_rot = VECTOR3_NULL;		// 角度
	float m_fRadiusInner = 0.0f;		// 内径
	float m_fRingWidth = 0.0f;			// リングの幅
	int m_nNumBlockRing = 0;		// リングの分割数
	Matrix m_mtxWorld;		// ワールドマトリックス
};
#endif