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
#include "core/resourceable.h"
#include "core/singleton.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** テクスチャクラス ***
//**********************************************************************************
class CTexture : public ITypedResourceable<IDirect3DTexture9>, public ISingleton<CTexture>
{
	friend ISingleton<CTexture>;

public:
	// 事前に読み込むテクスチャの種類
	enum class TYPE
	{
		TRIANGLE = 0,	// 三角図形
		CIRCLE,			// 円形
		DOUGHNUT,		// ドーナツ形
		STAR,			// 星形
		NOTE,			// 音符型
		ARROW,			// 矢印型
		MAX
	};

	~CTexture();

	void Load(void) override;
	void Unload(void) override;
	UINT PreRegister(std::string_view path) override;
	LPDIRECT3DTEXTURE9 GetAddress(const UINT uIdx) override;

private:
	// テクスチャデータ構造体
	struct Buffer
	{
		LPDIRECT3DTEXTURE9 pTexture = nullptr;	// テクスチャポインタ
		std::string sFilename;			// ファイル名
	};

	CTexture();

	UINT RegisterImpl(std::string_view path) override;
	UINT Load(std::string_view path);

	std::vector<std::string> m_vsPathPreLoad;		// 読み込み予定のテクスチャパス
	std::vector<Buffer> m_vTexBuff;			// テクスチャの配列
};
#endif