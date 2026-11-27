//==================================================================================
// 
// シェーダー管理クラスのソースファイル [shader.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "render/shader.h"
#include "core/manager.h"
#include "core/serviceLocator.h"
#include "render/renderer.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int DEFAULT_CAPACITY = 64;		// インスタンス生成時確保するTEX_BUFFERの初期サイズ
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CShader::CShader()
{ // バッファの事前確保
	m_vShader.reserve(DEFAULT_CAPACITY);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CShader::~CShader()
{
}

//==================================================================================
// --- 事前登録済みのシェーダー読み込み処理 ---
//==================================================================================
void CShader::Load(void)
{
	for (const auto &path : m_vsPathPreLoad)
	{ // 事前に登録されたシェーダーを読み込み
		Register(path);
	}
}

//==================================================================================
// --- 全シェーダーの解放処理 ---
//==================================================================================
void CShader::Unload(void)
{
	for (auto &shaderbuf : m_vShader)
	{ // 読み込んだテクスチャの解放
		SafeRelease(shaderbuf.pShader);

		// ファイル名をクリア
		shaderbuf.sFilename.clear();
	}

	// 配列をクリア
	m_vShader.clear();
}

//==================================================================================
// --- シェーダーの登録処理 ---
//==================================================================================
UINT CShader::RegisterImpl(std::string_view path)
{ // ファイル名がnullもしくはインデックスが無効値手前の場合失敗
	if (path.empty()) return INVALID_RESOURCE_ID;
	if (m_vShader.size() == INVALID_RESOURCE_ID - 1U) return INVALID_RESOURCE_ID;

	// 既に読み込んでいないかを確認
	for (UINT uCntShader = 0; uCntShader < m_vShader.size(); uCntShader++)
	{
		if (m_vShader[uCntShader].sFilename == path)
		{ // 既に読み込み済みのシェーダーなら、そのインデックスを返す
			return uCntShader;
		}
	}

	// 新規で読み込み
	return Load(path);
}

//==================================================================================
// --- シェーダーの事前登録処理 ---
//==================================================================================
UINT CShader::PreRegister(std::string_view path)
{ // 登録済みならインデックスを返す
	for (int uCntShader = 0; uCntShader < static_cast<int>(m_vsPathPreLoad.size()); uCntShader++)
	{ // 既に登録されていないかを確認
		if (m_vsPathPreLoad[static_cast<UINT>(uCntShader)] == path)
		{ // 既に読み込み済みのシェーダーなら、そのインデックスを返す
			return static_cast<UINT>(uCntShader);
		}
	}

	UINT uIdx = m_vsPathPreLoad.size();					// インデックス
	m_vsPathPreLoad.push_back(std::string(path));		// パスを登録
	return uIdx;
}

//==================================================================================
// --- 読み込んだシェーダーの取得処理 ---
//==================================================================================
LPD3DXEFFECT CShader::GetAddress(const UINT uIdx)
{ // 無効なインデックスならnullptrを返す
	if (uIdx >= m_vShader.size() || IResourceable::IsValid(uIdx) == false) return nullptr;

	// シェーダーへのポインタを返す
	return m_vShader[uIdx].pShader;
}

//==================================================================================
// --- シェーダーの読み込み処理 ---
//==================================================================================
UINT CShader::Load(std::string_view path)
{
	LPDIRECT3DDEVICE9 pDevice = CServiceLocator::GetDevice();	// テクスチャの読み込み
	UINT uIdxTex = m_vShader.size();		// シェーダーのインデックス
	LPD3DXBUFFER *ppError = nullptr;	// エラー文取得の分岐用ポインタ
	HRESULT hr = S_OK;			// 処理結果
	Buffer buf = {};			// シェーダーのバッファ
	DWORD dwShaderFlags = 0;	// シェーダーフラグ

#ifdef _DEBUG
	LPD3DXBUFFER pError = nullptr;		// エラー文取得用ポインタ
	ppError = &pError;		// ポインタのアドレスを取得

	// デバッグビルド時はデバッグ情報を付与し最適化を無効化
	dwShaderFlags |= D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
#endif

	// シェーダーを読み込み
	hr = D3DXCreateEffectFromFile(pDevice,		// Direct3DDeviceへのポインタ
		path.data(),		// fxファイル名
		NULL,				// プリプロセッサマクロ定義へのポインタ(基本NULL)
		NULL,				// fxファイル内でインクルードする場合のポインタ(基本NULL)
		dwShaderFlags,		// シェーダーフラグ
		NULL,				// シェーダーのプール指定(共有しない場合NULL)
		&buf.pShader,		// エフェクトバッファへのダブルポインタ
		ppError);			// エラー文出力先
#ifdef _DEBUG
	if (pError != nullptr)
	{ // エラー文を取得可能なら
		const char *pErrorStr = reinterpret_cast<const char*>(pError->GetBufferPointer());		// エラー文
		MessageBox(CServiceLocator::GetWindowHandle(),
			pErrorStr,
			"Shader Error",
			MB_ICONERROR);
	}
#endif
	if (FAILED(hr))
	{ // 読み込み失敗時、無効値を返す
		return INVALID_RESOURCE_ID;
	}
	
	// ファイル名を保存
	buf.sFilename.append(path);

	// 配列に保存
	m_vShader.push_back(buf);

	// インデックスを返す
	return uIdxTex;
}