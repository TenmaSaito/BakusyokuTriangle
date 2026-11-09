//==================================================================================
// 
// Xファイル管理クラスのヘッダーファイル [xfile.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _XFILE_H_		// インクルードガード
#define _XFILE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr UINT INVALID_XFILE_ID = static_cast<UINT>(-1);		// XFILEの無効値

//**********************************************************************************
// *** Xファイルクラス ***
//**********************************************************************************
class CXFile
{
public:
	// 前方宣言
	class CXInfo;

	static CXFile *GetInstance(void);

	void Load(void);
	void Unload(void);
	UINT Register(std::string_view path);
	UINT Register(const char *pPath);
	UINT PreRegister(std::string path);
	CXInfo *GetAddress(const UINT uIdx);

private:

	CXFile();
	~CXFile();

	UINT IsAlreadyRegister(std::string_view path);
	UINT Load(std::string_view path);

	std::vector<std::string> m_vsPathPreLoad;		// 読み込み予定のモデルパス
	std::vector<CXInfo> m_vXInfo;			// XInfoの配列
};

//**********************************************************************************
// *** XFILE関連情報クラス ***
//**********************************************************************************
class CXFile::CXInfo
{
public:
	// Init,Uninit関数呼び出し用
	friend UINT CXFile::Load(const std::string_view path);
	friend void CXFile::Unload(void);

	CXInfo() = default;
	~CXInfo() = default;

	void DrawSubset(DWORD dwAttribId) { m_pMesh->DrawSubset(dwAttribId); }
	int GetMatrialNum(void) const { return m_dwNumMat; }
	const D3DMATERIAL9 *GetMaterial(DWORD dwAttribId) const { return &m_vMat[dwAttribId]; }
	int GetTextureIdx(DWORD dwAttribId) const { return m_vIdx[dwAttribId]; }
	std::string_view GetFilePath(void) const { return m_sFilePath; }
	const Vector3 *GetVtxMin(void) const { return &m_vtxMin; }
	const Vector3 *GetVtxMax(void) const { return &m_vtxMax; }
	bool IsHitByRay(const Vector3 &start,
		const Vector3 &vec,
		const float fLength,
		const Matrix *pMtxInv);

private:
	HRESULT Init(std::string_view path);
	void Uninit(void);

	LPD3DXMESH m_pMesh = nullptr;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat = nullptr;		// マテリアルへのポインタ
	std::vector<D3DMATERIAL9> m_vMat;		// マテリアルの配列
	std::vector<int> m_vIdx;				// テクスチャインデックスの配列
	std::string m_sFilePath;				// Xファイルパス
	DWORD m_dwNumMat = 0U;					// マテリアルの数
	Vector3 m_vtxMin = VECTOR3_NULL;		// モデルの各最小頂点の位置
	Vector3	m_vtxMax = VECTOR3_NULL;		// モデルの各最大頂点の位置
};
#endif
