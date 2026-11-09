//==================================================================================
// 
// オブジェクト3Dクラスのソースファイル [object3D.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/object3D.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "math/matrix.h"
#include "math/vec3math.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CObject3D *CObject3D::Create(const Vector3 &pos,
	const Vector3 &rot, 
	const Vector2 &size,
	const bool bXYPlane)
{
	CObject3D *pObject3D = new CObject3D;		// 生成したオブジェクトへのポインタ
	if (pObject3D != nullptr)
	{ // 初期化処理
		pObject3D->Init(pos, rot, size, bXYPlane);
	}

	return pObject3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObject3D::CObject3D(const int nPriority) : CObject(nPriority)
{ // タイプを指定
	SetType(TYPE::OBJ_3D);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObject3D::~CObject3D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObject3D::Init(const Vector3 &pos, 
	const Vector3 &rot,
	const Vector2 &size,
	const bool bXYPlane)
{
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	HRESULT hr;						// テクスチャ読み込みの判定
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

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

	// 引数の保存
	m_pos = pos;
	m_rot = rot;
	m_size = size;
	m_bXYPlane = bXYPlane;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = -m_size.x * 0.5f;
	pVtx[1].pos.x = m_size.x * 0.5f;
	pVtx[2].pos.x = -m_size.x * 0.5f;
	pVtx[3].pos.x = m_size.x * 0.5f;
	
	if (bXYPlane == false)
	{ // XZ平面の場合
		pVtx[0].pos.y = 0.0f;
		pVtx[1].pos.y = 0.0f;
		pVtx[2].pos.y = 0.0f;
		pVtx[3].pos.y = 0.0f;

		pVtx[0].pos.z = m_size.y * 0.5f;
		pVtx[1].pos.z = m_size.y * 0.5f;
		pVtx[2].pos.z = -m_size.y * 0.5f;
		pVtx[3].pos.z = -m_size.y * 0.5f;

		// 法線ベクトル設定
		pVtx[0].nor = Vector3(0.0f, -1.0f, 0.0f);
		pVtx[1].nor = Vector3(0.0f, -1.0f, 0.0f);
		pVtx[2].nor = Vector3(0.0f, -1.0f, 0.0f);
		pVtx[3].nor = Vector3(0.0f, -1.0f, 0.0f);
	}
	else
	{ // XY平面の場合
		pVtx[0].pos.y = m_size.y * 0.5f;
		pVtx[1].pos.y = m_size.y * 0.5f;
		pVtx[2].pos.y = -m_size.y * 0.5f;
		pVtx[3].pos.y = -m_size.y * 0.5f;

		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;

		// 法線ベクトル設定
		pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);
	}

	// 頂点カラー設定
	pVtx[0].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = Color(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObject3D::Uninit(void)
{ // 頂点バッファの破棄
	SafeRelease(m_pVtxBuff);

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObject3D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObject3D::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_rot);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- 位置の変更処理 ---
//==================================================================================
void CObject3D::SetPosition(const Vector3 &position)
{ // 位置の変更及び変更フラグを立てる
	m_pos = position;
}

//==================================================================================
// --- 角度の変更処理 ---
//==================================================================================
void CObject3D::SetRotation(const Vector3 &rotation)
{ // 角度の変更及び変更フラグを立てる
	m_rot = rotation;
}

//==================================================================================
// --- サイズの変更処理 ---
//==================================================================================
void CObject3D::SetSize(const Vector2& size)
{ // サイズの変更及び変更フラグを立てる
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	m_size = size;		// サイズを保存

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = (m_size.x * 0.5f);
	pVtx[1].pos.x = (m_size.x * 0.5f);
	pVtx[2].pos.x = (m_size.x * 0.5f);
	pVtx[3].pos.x = (m_size.x * 0.5f);
	
	pVtx[0].pos.y = 0.0f;
	pVtx[1].pos.y = 0.0f;
	pVtx[2].pos.y = 0.0f;
	pVtx[3].pos.y = 0.0f;

	pVtx[0].pos.z = (m_size.y * 0.5f);
	pVtx[1].pos.z = (m_size.y * 0.5f);
	pVtx[2].pos.z = (m_size.y * 0.5f);
	pVtx[3].pos.z = (m_size.y * 0.5f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- マトリックスの取得処理 ---
//==================================================================================
const Matrix *CObject3D::GetMatrix(void)
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
// --- 色変更処理 ---
//==================================================================================
void CObject3D::SetColor(const Color &col)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}