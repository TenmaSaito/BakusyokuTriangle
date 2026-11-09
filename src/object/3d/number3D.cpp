//==================================================================================
// 
// 数値3D表示クラスのソースファイル [number3D.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/number3D.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "math/matrix.h"
#include <string_view>

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float NUMBER_UV_WIDTH = 1.0f / DECIMAL_BASE_F;	// 数字1つ分のテクスチャ幅
}

//==================================================================================
// --- 数値オブジェクトの生成処理 (位置、サイズ、値指定) ---
//==================================================================================
CNumber3D *CNumber3D::Create(const Vector3 &pos,
	const Vector2 &size,
	const int nValue)
{
	CNumber3D *pNumber3D = new CNumber3D;		// 生成したオブジェクトへのポインタ
	if (pNumber3D != nullptr)
	{ // 生成に成功した場合、初期化
		pNumber3D->Init(pos, size, nValue);
	}

	return pNumber3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CNumber3D::CNumber3D()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CNumber3D::~CNumber3D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CNumber3D::Init(const Vector3 &pos,
	const Vector2 &size,
	const int nNumber)
{
	HRESULT hr;						// テクスチャ読み込みの判定
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 各引数を保存
	m_pos = pos;
	m_size = size;
	m_nNumber = nNumber;
	m_col = COLOR_ONE;

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * DEFAULT_VERTEX_NUM,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = -m_size.x * 0.5f;
	pVtx[0].pos.y = m_size.y * 0.5f;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_size.x * 0.5f;
	pVtx[1].pos.y = m_size.y * 0.5f;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = -m_size.x * 0.5f;
	pVtx[2].pos.y = -m_size.y * 0.5f;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_size.x * 0.5f;
	pVtx[3].pos.y = -m_size.y * 0.5f;
	pVtx[3].pos.z = 0.0f;

	// 法線ベクトル設定
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);

	// 頂点カラー設定
	pVtx[0].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = Color(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	int nUV = m_nNumber % DECIMAL_BASE;
	pVtx[0].tex = Vector2(NUMBER_UV_WIDTH * nUV, 0.0f);
	pVtx[1].tex = Vector2(NUMBER_UV_WIDTH * nUV + NUMBER_UV_WIDTH, 0.0f);
	pVtx[2].tex = Vector2(NUMBER_UV_WIDTH * nUV, 1.0f);
	pVtx[3].tex = Vector2(NUMBER_UV_WIDTH * nUV + NUMBER_UV_WIDTH, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CNumber3D::Uninit(void)
{ // 頂点バッファの破棄
	SafeRelease(m_pVtxBuff);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CNumber3D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CNumber3D::Draw(void)
{ // 描画しない場合はスキップ
	if (m_bDisp == false) return;

	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// マトリックス計算
	Mtx::Identity(&m_mtxWorld);
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_rot);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- 描画処理 (マトリックス外部計算) ---
//==================================================================================
void CNumber3D::Draw(const Matrix &mtx)
{
	// マトリックスを保存
	m_mtxWorld = mtx;

	// 描画しない場合はスキップ
	if (m_bDisp == false) return;

	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- サイズ設定処理 ---
//==================================================================================
void CNumber3D::SetSize(const Vector2 &size)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// サイズを保存
	m_size = size;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = -m_size.x * 0.5f;
	pVtx[0].pos.y = m_size.y * 0.5f;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_size.x * 0.5f;
	pVtx[1].pos.y = m_size.y * 0.5f;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = -m_size.x * 0.5f;
	pVtx[2].pos.y = -m_size.y * 0.5f;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_size.x * 0.5f;
	pVtx[3].pos.y = -m_size.y * 0.5f;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 色設定処理 ---
//==================================================================================
void CNumber3D::SetColor(const Color &col)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	m_col = col;		// 色を保存

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

	// 色を設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 値設定処理 ---
//==================================================================================
void CNumber3D::SetNumber(const int nValue)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 値を保存
	m_nNumber = nValue;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標設定
	int nUV = m_nNumber % DECIMAL_BASE;
	pVtx[0].tex = Vector2(NUMBER_UV_WIDTH * nUV, 0.0f);
	pVtx[1].tex = Vector2(NUMBER_UV_WIDTH * nUV + NUMBER_UV_WIDTH, 0.0f);
	pVtx[2].tex = Vector2(NUMBER_UV_WIDTH * nUV, 1.0f);
	pVtx[3].tex = Vector2(NUMBER_UV_WIDTH * nUV + NUMBER_UV_WIDTH, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}