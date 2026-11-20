//==================================================================================
// 
// ステンシルシャドウクラスのヘッダーファイル [shadowSt.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _SHADOW_ST_H_		// インクルードガード
#define _SHADOW_ST_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/objectX.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** ステンシルシャドウクラス ***
//**********************************************************************************
class CShadowSt : public CObjectX
{
public:
	CShadowSt();
	~CShadowSt();

	static CShadowSt *Create(const Matrix *pTarget,
		std::string_view sModelPath = DEFAULT_SHADOW_MODEL_PATH);

	HRESULT Init(const Matrix *pTarget,
		std::string_view sModelPath);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetOffset(const Vector3 &offset) { m_offset = offset; }

private:
	static constexpr std::string_view DEFAULT_SHADOW_MODEL_PATH = "data/MODEL/shadowSt.x";		// デフォルト影モデル
	std::unique_ptr<CPolygon2D> m_pPolygon;			// 画面全体を覆うポリゴン
	Vector3 m_offset = VECTOR3_NULL;				// オフセット
};
#endif