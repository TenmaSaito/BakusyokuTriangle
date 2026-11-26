//==================================================================================
//
// パーティクル(バッチ描画)クラスのソースファイル [particleBatch.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "effect/particleBatch.h"
#include "core/serviceLocator.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CParticleBatch::CParticleBatch(const int nPriority) : CObject(nPriority)
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CParticleBatch::~CParticleBatch()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CParticleBatch::Init(const Setting &setting, const int nMaxEffect)
{
	LPDIRECT3DDEVICE9 pDevice = CServiceLocator::GetDevice();		// デバイス
	HRESULT hr = S_OK;		// 処理結果

	// 頂点バッファ生成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * DEFAULT_VERTEX_NUM * nMaxEffect,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);
	if (FAILED(hr))
	{ // 生成失敗
		return hr;
	}

	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ
	hr = m_pVtxBuff->Lock(0, 0, reinterpret_cast<void**>(&pVtx), 0);
	if (SUCCEEDED(hr))
	{ // ロック成功

		// アンロック
		m_pVtxBuff->Unlock();
	}

	// インデックスバッファ生成
	hr = pDevice->CreateIndexBuffer(sizeof(WORD) * nMaxEffect * 6,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		nullptr);
	if (FAILED(hr))
	{ // 生成失敗
		return hr;
	}

	WORD *pIdx = nullptr;		// インデックスへのポインタ
	hr = m_pIdxBuff->Lock(0, 0, reinterpret_cast<void **>(&pIdx), 0);
	if (SUCCEEDED(hr))
	{ // ロック成功

		// アンロック
		m_pIdxBuff->Unlock();
	}

	return hr;
}

//==================================================================================
// --- 頂点初期化処理 ---
//==================================================================================
void CParticleBatch::InitVertex(VERTEX_3D *pVtx, const int nMaxEffect)
{
	for (int nCntEffect = 0; nCntEffect < nMaxEffect; nCntEffect++)
	{
		pVtx[0].pos = Vector3();
		pVtx[1].pos = Vector3();
		pVtx[2].pos = Vector3();
		pVtx[3].pos = Vector3();

		pVtx += 4;
	}
}

//==================================================================================
// --- インデックス初期化処理 ---
//==================================================================================
void CParticleBatch::InitIndex(WORD *pItx)
{

}