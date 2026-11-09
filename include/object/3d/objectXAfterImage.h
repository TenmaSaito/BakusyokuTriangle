//==================================================================================
// 
// オブジェクトXの残像クラスのヘッダーファイル [objectXAfterImage.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _OBJECTX_AFTERIMAGE_H_		// インクルードガード
#define _OBJECTX_AFTERIMAGE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "object/base/parentable.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int DEFAULT_OBJX_AFTERIMAGE_PRIORITY = DEFAULT_OBJ_PRIORITY;		// objXの残像の基本優先順位	

//**********************************************************************************
// *** オブジェクトXの残像クラス ***
//**********************************************************************************
class CObjectXAfterImage : public CObject, public IParentable
{
public:
	CObjectXAfterImage(const int nPriority = DEFAULT_OBJX_AFTERIMAGE_PRIORITY);
	~CObjectXAfterImage();

	static CObjectXAfterImage *Create(std::string_view pFilename,
		const Matrix &mtxWorld,
		const int nLife,
		const float fStartAlpha);

	HRESULT Init(std::string_view pFilename,
		const Matrix &mtxWorld,
		const int nLife,
		const float fStartAlpha);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return m_pMtxParent; }

private:
	const Matrix *m_pMtxParent = nullptr;	// 親マトリックスへのポインタ
	Matrix m_mtxWorld;			// ワールドマトリックス
	float m_fAlpha = 0.0f;		// α値
	float m_fDecrease = 0.0f;	// α値の一フレーム当たりの減少値
	int m_nLife = 0;			// 体力
	UINT m_uIdxXInfo = 0U;		// XFILE情報へのインデックス
};
#endif