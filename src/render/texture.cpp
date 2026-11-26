//==================================================================================
// 
// テクスチャクラスのソースファイル [texture.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "render/texture.h"
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

//**********************************************************************************
// *** 図形のファイル名 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asPrimitivePath[EnumUtil::ToIndex(CTexture::TYPE::MAX)] =	// 図形テクスチャのパス
	{
		"data/TEXTURE/base/triangle.png",	// 三角形
		"data/TEXTURE/base/circle.png",		// 円形
		"data/TEXTURE/base/doughnut.png",	// ドーナツ形
		"data/TEXTURE/base/star.png",		// 星型
		"data/TEXTURE/base/note.png",		// 音符型
		"data/TEXTURE/base/arrow.png",		// 矢印型
	};
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTexture::CTexture()
{ // バッファの事前確保
	m_vTexBuff.reserve(DEFAULT_CAPACITY);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTexture::~CTexture()
{
}

//==================================================================================
// --- 既存テクスチャの読み込み処理 ---
//==================================================================================
void CTexture::Load(void)
{
	for (const auto &path : c_asPrimitivePath)
	{ // 事前に用意されたテクスチャを読み込み
		Register(path);
	}
}

//==================================================================================
// --- 全テクスチャの解放処理 ---
//==================================================================================
void CTexture::Unload(void)
{
	for (auto &texbuf : m_vTexBuff)
	{ // 読み込んだテクスチャの解放
		if (texbuf.pTexture != nullptr)
		{ // テクスチャが読み込まれていた場合
			texbuf.pTexture->Release();
			texbuf.pTexture = nullptr;
		}

		// ファイル名をクリア
		texbuf.sFilename.clear();
	}

	// 配列をクリア
	m_vTexBuff.clear();
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
UINT CTexture::RegisterImpl(std::string_view path)
{ // ファイル名がnullもしくはインデックスが無効値手前の場合失敗
	if (path.empty()) return INVALID_RESOURCE_ID;
	if (m_vTexBuff.size() == INVALID_RESOURCE_ID - 1U) return INVALID_RESOURCE_ID;

	// 既に読み込んでいないかを確認
	for (UINT uCntTexture = 0; uCntTexture < m_vTexBuff.size(); uCntTexture++)
	{
		if (m_vTexBuff[uCntTexture].sFilename == path)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return uCntTexture;
		}
	}

	// 新規で読み込み
	return Load(path);
}

//==================================================================================
// --- テクスチャの事前登録処理 ---
//==================================================================================
UINT CTexture::PreRegister(std::string_view path)
{ // 登録済みならインデックスを返す
	for (int nCntTexture = 0; nCntTexture < static_cast<int>(m_vsPathPreLoad.size()); nCntTexture++)
	{ // 既に登録されていないかを確認
		if (m_vsPathPreLoad[static_cast<UINT>(nCntTexture)] == path)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return static_cast<UINT>(nCntTexture);
		}
	}

	UINT uIdx = m_vsPathPreLoad.size();					// インデックス
	m_vsPathPreLoad.push_back(std::string(path));		// パスを登録
	return uIdx;
}

//==================================================================================
// --- 読み込んだテクスチャの読み込み処理 ---
//==================================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(const UINT uIdx)
{ // 無効なインデックスならnullptrを返す
	if (uIdx >= m_vTexBuff.size() || IResourceable::IsValid(uIdx) == false) return nullptr;

	// テクスチャへのポインタを返す
	return m_vTexBuff[uIdx].pTexture;
}

//==================================================================================
// --- テクスチャの読み込み処理 ---
//==================================================================================
UINT CTexture::Load(std::string_view path)
{
	LPDIRECT3DDEVICE9 pDevice = CServiceLocator::GetDevice();	// テクスチャの読み込み
	UINT uIdxTex = m_vTexBuff.size();		// テクスチャのインデックス
	HRESULT hr = S_OK;			// 処理結果
	Buffer texbuf = {};			// テクスチャのバッファ

	// テクスチャを読み込み
	hr = D3DXCreateTextureFromFile(pDevice,
		path.data(),
		&texbuf.pTexture);
	if (FAILED(hr))
	{ // 読み込み失敗時、無効値を返す
		return INVALID_RESOURCE_ID;
	}
	
	// ファイル名を保存
	texbuf.sFilename.append(path);

	// 配列に保存
	m_vTexBuff.push_back(texbuf);

	// インデックスを返す
	return uIdxTex;
}