//==================================================================================
// 
// Xファイル管理クラスのヘッダーファイル [xfile.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "render/xfile.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "math/vec3math.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int DEFAULT_CAPACITY = 64;		// インスタンス生成時確保するTEX_BUFFERの初期サイズ
}

//==================================================================================
// --- インスタンス取得処理 ---
//==================================================================================
CXFile *CXFile::GetInstance(void)
{
	static CXFile instance;		// インスタンス
	return &instance;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CXFile::CXFile()
{ // バッファの事前確保
	m_vXInfo.reserve(DEFAULT_CAPACITY);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CXFile::~CXFile()
{
}

//==================================================================================
// --- 事前登録済みのXファイル読み込み処理 ---
//==================================================================================
void CXFile::Load(void)
{
	for (auto &sPath : m_vsPathPreLoad)
	{ // XFileの読み込み
		Register(sPath);
	}
}

//==================================================================================
// --- 全XFILEの解放処理 ---
//==================================================================================
void CXFile::Unload(void)
{
	for (auto &xInfo : m_vXInfo)
	{ // 読み込んだXFileの解放
		xInfo.Uninit();
	}

	// 配列をクリア
	m_vXInfo.clear();
}

//==================================================================================
// --- XFILEの登録処理 ---
//==================================================================================
UINT CXFile::Register(std::string_view path)
{ // ファイル名がnullもしくはインデックスが無効値手前の場合失敗
	if (path.empty()) return INVALID_XFILE_ID;
	if (m_vXInfo.size() == INVALID_XFILE_ID - 1U) return INVALID_XFILE_ID;

	// 既に読み込んでいないかを確認
	for (int nCntXFile = 0; nCntXFile < static_cast<int>(m_vXInfo.size()); nCntXFile++)
	{
		if (m_vXInfo[nCntXFile].GetFilePath() == path)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return static_cast<UINT>(nCntXFile);
		}
	}

	// 新規で読み込み
	return Load(path);
}

//==================================================================================
// --- XFILEの登録処理 (nullptr対策) ---
//==================================================================================
UINT CXFile::Register(const char *pPath)
{ // nullptrの場合、無効値を渡す
	if (pPath == nullptr) return INVALID_XFILE_ID;

	// 存在すれば正式に処理を呼び出す
	return Register(std::string_view(pPath));
}

//==================================================================================
// --- XFILEの事前登録処理 ---
//==================================================================================
UINT CXFile::PreRegister(std::string path)
{ // 登録済みならインデックスを返す
	if (UINT uIdx = IsAlreadyRegister(path) != -1) return uIdx;

	UINT uIdx = m_vsPathPreLoad.size();		// インデックス
	m_vsPathPreLoad.push_back(path);		// パスを登録
	return uIdx;
}

//==================================================================================
// --- 読み込んだXFILEの取得処理 ---
//==================================================================================
CXFile::CXInfo *CXFile::GetAddress(const UINT uIdx)
{ // 無効なインデックスならnullptrを返す
	if (uIdx >= m_vXInfo.size() || uIdx == INVALID_XFILE_ID) return nullptr;

	// XInfoへのポインタを返す
	return &m_vXInfo[uIdx];
}

//==================================================================================
// --- 登録済み判定処理 ---
//==================================================================================
UINT CXFile::IsAlreadyRegister(std::string_view path)
{
	// 既に読み込んでいないかを確認
	for (int nCntXFile = 0; nCntXFile < static_cast<int>(m_vXInfo.size()); nCntXFile++)
	{
		if (m_vXInfo[nCntXFile].GetFilePath() == path)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return static_cast<UINT>(nCntXFile);
		}
	}

	return INVALID_XFILE_ID;
}

//==================================================================================
// --- XFILEの読み込み処理 ---
//==================================================================================
UINT CXFile::Load(std::string_view path)
{
	UINT uIdx = m_vXInfo.size();		// 今回のインデックス
	CXInfo &rNewXInfo = m_vXInfo.emplace_back();		// XInfoを新規作成
	
	// ファイル名を渡して初期化
	rNewXInfo.Init(path);
	return uIdx;
}

//**********************************************************************************
// *** XFILE関連情報クラス ***
//**********************************************************************************
//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CXFile::CXInfo::Init(std::string_view path)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = nullptr;	// マテリアルへのポインタ
	HRESULT hr = S_OK;				// モデル読み込み結果
	int nNumVtx = 0;				// 頂点数
	DWORD dwSizeFVF = 0;			// 頂点フォーマットのサイズ
	BYTE *pVtxBuff = nullptr;		// 頂点バッファへのポインタ

	// Xファイルの読み込み
	hr = D3DXLoadMeshFromX(path.data(),			// 読み込むXファイル名
		D3DXMESH_SYSTEMMEM,
		pDevice,		// デバイスポインタ
		NULL,
		&m_pBuffMat,	// マテリアルへのポインタ
		NULL,
		&m_dwNumMat,	// マテリアルの数
		&m_pMesh);		// メッシュへのポインタ
	if (FAILED(hr))
	{ // 読み込み失敗
		return E_FAIL;
	}

	// マテリアル数分だけ、インデックス用バッファとマテリアル用バッファを確保
	m_vIdx.reserve(m_dwNumMat);
	m_vMat.reserve(m_dwNumMat);

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{ // マテリアル数分だけテクスチャチェック
		// テクスチャの読み込み
		m_vIdx.emplace_back(pTexture->Register(pMat[nCntMat].pTextureFilename));

		// マテリアルを保存
		m_vMat.push_back(pMat[nCntMat].MatD3D);
	}

	// 頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3 *)pVtxBuff;	// 頂点座標の代入

		// 最小値を取得
		m_vtxMin.x = (m_vtxMin.x > vtx.x) ? vtx.x : m_vtxMin.x;
		m_vtxMin.y = (m_vtxMin.y > vtx.y) ? vtx.y : m_vtxMin.y;
		m_vtxMin.z = (m_vtxMin.z > vtx.z) ? vtx.z : m_vtxMin.z;

		// 最大値を取得
		m_vtxMax.x = (m_vtxMax.x < vtx.x) ? vtx.x : m_vtxMax.x;
		m_vtxMax.y = (m_vtxMax.y < vtx.y) ? vtx.y : m_vtxMax.y;
		m_vtxMax.z = (m_vtxMax.z < vtx.z) ? vtx.z : m_vtxMax.z;

		pVtxBuff += dwSizeFVF;		// 頂点フォーマットのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	m_pMesh->UnlockVertexBuffer();

	// ファイル名保存
	m_sFilePath = path;

	// 読み込み結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CXFile::CXInfo::Uninit(void)
{
	// メッシュを解放
	SafeRelease(m_pMesh);

	// マテリアルを解放
	SafeRelease(m_pBuffMat);

	// インデックスをクリア
	m_vIdx.clear();
}

//==================================================================================
// --- レイとの処衝突判定処理 ---
//==================================================================================
bool CXFile::CXInfo::IsHitByRay(const Vector3 &start,
	const Vector3 &vec, 
	const float fLength,
	const Matrix *pMtxInv)
{
	Vector3 posLocal;			// ローカル座標系へ変換した始点
	Vector3 posLocalRay;		// ローカル座標系へ変換したレイベクトル
	BOOL bResult = FALSE;		// 衝突判定結果
	FLOAT fLengthToStart;		// 衝突座標との距離

	// 逆変換マトリックスで座標をローカル座標へ変換
	D3DXVec3TransformCoord(&posLocal, &start, pMtxInv);
	D3DXVec3TransformNormal(&posLocalRay, &vec, pMtxInv);
	posLocalRay = Vec3::Normalize(posLocalRay);

	// 変換したレイベクトルとオブジェクトの衝突判定
	if (FAILED(D3DXIntersect(m_pMesh,
		&posLocal,
		&posLocalRay,
		&bResult,
		nullptr,
		nullptr,
		nullptr,
		&fLengthToStart,
		nullptr,
		nullptr)))
	{ // 判定失敗
		return false;
	}
	else
	{ // boolへ変換
		return (bResult != FALSE && fLengthToStart < fLength);
	}
}