//==================================================================================
// 
// オブジェクトXクラスのソースファイル [objectX.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/objectX.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/light.h"
#include "render/texture.h"
#include "input/input.h"
#include "render/camera.h"
#include "math/vec2math.h"
#include "math/vec3math.h"
#include "math/matrix.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CObjectX *CObjectX::Create(std::string_view pFilename,
	const Vector3 &pos, 
	const Vector3 &rot)
{
	CObjectX *pObjectX = new CObjectX;		// 生成したオブジェクトへのポインタ
	if (pObjectX != nullptr)
	{ // 初期化処理
		pObjectX->Init(pFilename, pos, rot);
	}

	return pObjectX;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObjectX::CObjectX(const int nPriority) : CObject(nPriority)
{ // タイプの設定
	SetType(TYPE::OBJ_X);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObjectX::~CObjectX()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObjectX::Init(std::string_view pFilename,
	const Vector3 &pos,
	const Vector3 &rot)
{ // Xファイル読み込み
	LoadXFile(pFilename);

	// 引数の値を保存
	m_pos = pos;
	m_rot = rot;
	
	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObjectX::Uninit(void)
{ // メッシュを解放
	SafeRelease(m_pMesh);

	// マテリアルを解放
	SafeRelease(m_pBuffMat);

	// インデックスをクリア
	m_vIdx.clear();

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObjectX::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObjectX::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL *pMat = nullptr;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_rot);
	m_bMtxInitialize = true;		// マトリックスの計算フラグを立てる

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//  現在のマテリアルを保存
	pDevice->GetMaterial(&matDef);

	// マテリアルを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	// 各マテリアルを描画
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_vIdx[nCntMat]));

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- 描画処理 (マトリックス外部計算) ---
//==================================================================================
void CObjectX::Draw(const Matrix &mtx)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL *pMat = nullptr;		// マテリアルデータへのポインタ

	m_mtxWorld = mtx;			// ワールドマトリックスの代入
	m_bMtxInitialize = true;	// マトリックスの計算フラグを立てる

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//  現在のマテリアルを保存
	pDevice->GetMaterial(&matDef);

	// マテリアルを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	// 各マテリアルを描画
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_vIdx[nCntMat]));

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- 描画処理 (影) ---
//==================================================================================
void CObjectX::DrawShadow(const Matrix &mtxShadow)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL *pMat = nullptr;		// マテリアルデータへのポインタ

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

	//  現在のマテリアルを保存
	pDevice->GetMaterial(&matDef);

	// マテリアルを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	// 各マテリアルを描画
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{
		// マテリアルの設定
		D3DMATERIAL9 matShadow = pMat[nCntMat].MatD3D;			// マテリアルのコピー
		matShadow.Diffuse = Color(SHADOW_COLOR);		// マテリアルを黒色に変更

		pDevice->SetMaterial(&matShadow);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_vIdx[nCntMat]));

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- マトリックスの計算後取得処理 ---
//==================================================================================
const Matrix *CObjectX::GetMatrix(void)
{
	if (m_bMtxInitialize == false)
	{ // マトリックスの計算がされていなければ、計算
		Mtx::Identity(&m_mtxWorld);
		Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_rot);
		m_bMtxInitialize = true;	// マトリックスの計算フラグを立てる
	}

	return &m_mtxWorld;
}

//==================================================================================
// --- レイとの処衝突判定処理 ---
//==================================================================================
bool CObjectX::IsHitByRay(const Vector3 &start, const Vector3 &vec, const float fLength)
{
	Vector3 posLocal;			// ローカル座標系へ変換した始点
	Vector3 posLocalRay;		// ローカル座標系へ変換したレイベクトル
	BOOL bResult = FALSE;		// 衝突判定結果
	FLOAT fLengthToStart;		// 衝突座標との距離
	Matrix mtxInv;				// 逆行列

	// スケーリングを無視する為、マトリックスを再計算
	Mtx::Identity(&mtxInv);
	Mtx::CalcWorld(&mtxInv, m_pMtxParent, m_pos, m_rot);

	// ワールド座標系を自身のローカル座標系に逆変換するマトリックスを求める
	Mtx::Inverse(&mtxInv, mtxInv);

	// 逆変換マトリックスで座標をローカル座標へ変換
	D3DXVec3TransformCoord(&posLocal, &start, &mtxInv);
	D3DXVec3TransformNormal(&posLocalRay, &vec, &mtxInv);
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

//==================================================================================
// --- Xファイルの読み込み処理 ---
//==================================================================================
HRESULT	CObjectX::LoadXFile(std::string_view pFilename)
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
	hr = D3DXLoadMeshFromX(pFilename.data(),			// 読み込むXファイル名
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

	// マテリアル数分だけ、インデックス用バッファを確保
	m_vIdx.reserve(m_dwNumMat);

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{ // マテリアル数分だけテクスチャチェック
		// テクスチャの読み込み
		m_vIdx.emplace_back(pTexture->Register(pMat[nCntMat].pTextureFilename));
	}

	// 頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3*)pVtxBuff;	// 頂点座標の代入

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

	/*** 頂点バッファをアンロック ***/
	m_pMesh->UnlockVertexBuffer();

	// ファイル名保存
	m_sFilename = pFilename;

	// 読み込み結果を返す
	return S_OK;
}