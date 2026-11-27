//==================================================================================
// 
// シェーダー管理クラスのヘッダーファイル [shader.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _SHADER_H_		// インクルードガード
#define _SHADER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "core/resourceable.h"
#include "core/singleton.h"

//**********************************************************************************
// *** シェーダー管理クラス ***
//**********************************************************************************
class CShader : public ITypedResourceable<ID3DXEffect>, public ISingleton<CShader>
{
	friend ISingleton<CShader>;

public:
	~CShader();

	void Load(void) override;
	void Unload(void) override;
	UINT PreRegister(std::string_view path) override;
	LPD3DXEFFECT GetAddress(const UINT uIdx) override;

private:
	// シェーダーデータ構造体
	struct Buffer
	{
		LPD3DXEFFECT pShader = nullptr;	// シェーダーポインタ
		std::string sFilename;			// ファイル名
	};

	CShader();

	UINT RegisterImpl(std::string_view path) override;
	UINT IsAlreadyRegister(std::string_view path);
	UINT Load(std::string_view path);

	std::vector<std::string> m_vsPathPreLoad;	// 読み込み予定のシェーダーパス
	std::vector<Buffer> m_vShader;		// シェーダーデータの配列
};
#endif