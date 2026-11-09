//==================================================================================
// 
// テクスチャクラスのヘッダーファイル [texture.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _TEXTURE_H_		// インクルードガード
#define _TEXTURE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr UINT INVALID_TEX_ID = static_cast<UINT>(-1);		// テクスチャの無効値

//**********************************************************************************
// *** テクスチャクラス ***
//**********************************************************************************
class CTexture
{
public:
	// 事前に読み込むテクスチャの種類
	enum class TYPE
	{
		TRIANGLE = 0,		// 三角図形
		CIRCLE,			// 円形
		DOUGHNUT,			// ドーナツ形
		STAR,				// 星形
		NOTE,				// 音符型
		ARROW,				// 矢印型
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const TYPE value) noexcept
	{
		return EnumUtil::InRange(value, TYPE::TRIANGLE, TYPE::MAX);
	}

	// テクスチャデータ構造体
	struct BUFFER
	{
		LPDIRECT3DTEXTURE9 pTexture;	// テクスチャポインタ
		std::string sFilename;			// ファイル名
	};

	static CTexture *GetInstance(void);
	HRESULT Load(void);
	void Unload(void);
	UINT Register(std::string_view path);
	UINT Register(const char *pPath);
	LPDIRECT3DTEXTURE9 GetAddress(const UINT uIdx);

private:
	CTexture();
	~CTexture();

	UINT Load(std::string_view path);

	std::vector<BUFFER> m_vTexBuff;			// テクスチャの配列
};
#endif