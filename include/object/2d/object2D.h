//==================================================================================
// 
// 2Dオブジェクトクラスのヘッダーファイル [object2D.h]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
#ifndef _OBJECT2D_H_		// インクルードガード
#define _OBJECT2D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "object/base/object.h"

//**********************************************************************************
// *** オブジェクト2Dクラス ***
//**********************************************************************************
class CObject2D : public CObject
{
public:
	// ポリゴン作成時の情報更新構造体
	struct Info
	{
		Vector3 pos = VECTOR3_NULL;		// 位置
		Vector2 size = VECTOR2_NULL;	// サイズ
		std::string_view texturePath = {};		// テクスチャパス
	};

	CObject2D(const int nPriority = DEFAULT_OBJ2D_PRIORITY);
	~CObject2D();

	static CObject2D *Create(const Vector3 &pos);
	static CObject2D *Create(const Vector3 &pos, const Vector3 &rot);
	static CObject2D *Create(const Vector2 &size);
	static CObject2D *Create(const Vector3 &pos, const Vector2 &size);
	static CObject2D *Create(const Info &info);

	HRESULT Init(void);
	HRESULT Init(const Vector2 &size);
	HRESULT Init(const Vector3 &pos);
	HRESULT Init(const Vector3 &pos, const Vector3 &rot);
	HRESULT Init(const Vector3 &pos, const Vector2 &size);
	HRESULT Init(const Vector3 &pos, const Vector3 &rot, const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &position);
	const Vector3 &GetPosition(void) const { return m_pos; }
	void SetOffset(const Vector3 &offset);
	const Vector3 &GetOffset(void) const { return m_offset; }
	void SetRotation(const Vector3 &rotation);
	const Vector3 &GetRotation(void) const { return m_rot; }
	void SetPositionAndRotation(const Vector3 &position, const Vector3 &rotation);
	void SetSize(const Vector2 &size);
	const Vector2 &GetSize(void) const { return m_size; }
	void SetLength(const float fLength);
	float GetLength(void) const { return m_fLength; }
	void SetColor(const Color &color);
	const Color &GetColor(void) const { return m_color; }
	void SetAlpha(const float fAlpha) { SetColor(Color(m_color.r, m_color.g, m_color.b, fAlpha)); }
	float GetAlpha(void) const { return m_color.a; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:
	static constexpr int DEFAULT_OBJ2D_PRIORITY = DEFAULT_2DUI_PRIORITY;		// オブジェクト2Dの基本優先順位

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;			// 頂点バッファへのポインタ
	int m_nIdxTexture = -1;		// テクスチャインデックス
	Vector3 m_pos = VECTOR3_NULL;			// オブジェクトの位置
	Vector3 m_offset = VECTOR3_NULL;		// オブジェクト描画時のオフセット
	Vector3 m_rot = VECTOR3_NULL;			// オブジェクトの角度
	Vector2 m_size = VECTOR2_NULL;			// オブジェクトのサイズ
	Color m_color = COLOR_NULL;			// 色
	float m_fLength = 0.0f;		// 対角線の長さ
	float m_fAngle = 0.0f;			// 対角線の角度
	bool m_bDisp = false;			// 描画の有無
};
#endif