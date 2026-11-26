//==================================================================================
// 
// メッシュリングクラスのソースファイル [meshRing.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/meshRing.h"
#include "core/manager.h"
#include "core/serviceLocator.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "math/matrix.h"
#include "util/util.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int VERTICAL_VTX_NUM = 2;		// 縦の頂点数
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CMeshRing *CMeshRing::Create(const Vector3 &pos,
	const Vector3 &rot,
	const float fRadiusInner,
	const float fRingWidth,
	const int nNumBlockRing)
{
	return Util::Create<CMeshRing>(pos, rot, fRadiusInner, fRingWidth, nNumBlockRing);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMeshRing::CMeshRing(const int nPriority) : CObject(nPriority)
{ // タイプ指定
	SetType(TYPE::MESH_RING);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMeshRing::~CMeshRing()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMeshRing::Init(const Vector3 &pos,
	const Vector3 &rot,
	const float fRadiusInner,
	const float fRingWidth,
	const int nNumBlockRing)
{
	LPDIRECT3DDEVICE9 pDevice = CServiceLocator::GetDevice();		// デバイス
	HRESULT hr = S_OK;		// 結果

	// 引数を保存
	m_fRadiusInner = fRadiusInner;
	m_fRingWidth = fRingWidth;
	m_nNumBlockRing = nNumBlockRing;
	m_pos = pos;
	m_rot = rot;

	// 頂点バッファ生成
	hr = CreateVertex(pDevice);
	if (FAILED(hr))
	{ // 初期化失敗
		return hr;
	}

	// インデックスバッファ生成
	hr = CreateIndex(pDevice);
	if (FAILED(hr))
	{ // 初期化失敗
		return hr;
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMeshRing::Uninit(void)
{ // 頂点バッファとインデックスバッファ解放
	SafeRelease(m_pVtxBuff);
	SafeRelease(m_pIdxBuff);

	// オブジェクト解放
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMeshRing::Update(const ManagerContext::RenderContext &context)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMeshRing::Draw(const ManagerContext::RenderContext &context)
{
	LPDIRECT3DDEVICE9 pDevice = context.pDevice;		// デバイス
	CTexture *pTexture = context.pTexture;		// テクスチャ

	// マトリックスの初期化
	Mtx::Identity(&m_mtxWorld);

	// マトリックス計算
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_rot);

	// マトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);
	
	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	if (IsValidRenderState(false))
	{ // レンダーステートが登録されていれば設定
		SetRenderState(pDevice, false);
	}

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		(m_nNumBlockRing + 1) * 2,		// 頂点数
		0,
		m_nNumBlockRing * 2);	// 描画するプリミティブ(三角ポリゴン)の数

	if (IsValidRenderState(true))
	{ // レンダーステートが登録されていれば設定
		SetRenderState(pDevice, true);
	}
}

//==================================================================================
// --- 内径の設定処理 ---
//==================================================================================
void CMeshRing::SetRadiusInner(const float fRadiusInner)
{
	m_fRadiusInner = fRadiusInner;	// 内径を適用

	// 頂点座標を更新
	UpdateVertexPosition();
}

//==================================================================================
// --- リング幅の設定処理 ---
//==================================================================================
void CMeshRing::SetRingWidth(const float fRingWidth)
{
	m_fRingWidth = fRingWidth;		// リング幅を設定

	// 頂点座標を更新
	UpdateVertexPosition();
}

//==================================================================================
// --- 頂点設定処理 ---
//==================================================================================
HRESULT CMeshRing::CreateVertex(const LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hr = S_OK;		// 結果

	// 頂点バッファ生成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * ((m_nNumBlockRing + 1) * 2),
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);
	if (FAILED(hr))
	{ // 初期化失敗
		return hr;
	}

	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, reinterpret_cast<void**>(&pVtx), 0);

	int nNumVtxHorizontal = m_nNumBlockRing + 1;		// 円周の頂点数
	for (int nCntV = 0; nCntV < VERTICAL_VTX_NUM; nCntV++)
	{
		float fRadius = (nCntV == 0) ? m_fRadiusInner + m_fRingWidth : m_fRadiusInner;		// 半径

		for (int nCntH = 0; nCntH < nNumVtxHorizontal; nCntH++)
		{
			float fAngle = (DOUBLE_PI / (float)m_nNumBlockRing) * nCntH;		// 円周上の角度
			int nIdxVtx = nCntH + (nCntV * nNumVtxHorizontal);			// 頂点番号

			Vector3 posVtx = Vector3(sinf(fAngle) * fRadius, 0.0f, cosf(fAngle) * fRadius);	// 頂点座標

			// 頂点情報を設定
			pVtx[nIdxVtx].pos = posVtx;
			pVtx[nIdxVtx].nor = VECTOR3_UP;
			pVtx[nIdxVtx].col = Color(COLOR_ONE);
			pVtx[nIdxVtx].tex.x = (1.0f / static_cast<float>(m_nNumBlockRing)) * nCntH;
			pVtx[nIdxVtx].tex.y = static_cast<float>(nCntV);
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
	return hr;
}

//==================================================================================
// --- インデックス設定処理 ---
//==================================================================================
HRESULT CMeshRing::CreateIndex(const LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hr = S_OK;		// 結果

	// インデックスバッファ生成
	hr = pDevice->CreateIndexBuffer(sizeof(WORD) * ((m_nNumBlockRing + 1) * 2),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		nullptr);
	if (FAILED(hr))
	{ // 初期化失敗
		return hr;
	}

	WORD *pIdx = nullptr;		// インデックスへのポインタ

	// インデックスバッファをロック
	m_pIdxBuff->Lock(0, 0, reinterpret_cast<void**>(&pIdx), 0);

	for (int nCntH = 0; nCntH < m_nNumBlockRing + 1; nCntH++)
	{
		pIdx[0] = m_nNumBlockRing + 1 + nCntH;
		pIdx[1] = nCntH;

		pIdx += 2;
	}

	// インデックスバッファをアンロック
	m_pIdxBuff->Unlock();
	return hr;
}

//==================================================================================
// --- 頂点座標の更新処理 ---
//==================================================================================
void CMeshRing::UpdateVertexPosition(void)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, reinterpret_cast<void **>(&pVtx), 0);

	int nNumVtxHorizontal = m_nNumBlockRing + 1;		// 円周の頂点数
	for (int nCntV = 0; nCntV < VERTICAL_VTX_NUM; nCntV++)
	{
		float fRadius = (nCntV == 0) ? m_fRadiusInner + m_fRingWidth : m_fRadiusInner;		// 半径

		for (int nCntH = 0; nCntH < nNumVtxHorizontal; nCntH++)
		{
			float fAngle = (DOUBLE_PI / (float)m_nNumBlockRing) * nCntH;		// 円周上の角度
			int nIdxVtx = nCntH + (nCntV * nNumVtxHorizontal);			// 頂点番号

			Vector3 posVtx = Vector3(sinf(fAngle) * fRadius, 0.0f, cosf(fAngle) * fRadius);	// 頂点座標

			// 頂点情報を設定
			pVtx[nIdxVtx].pos = posVtx;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}