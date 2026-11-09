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
#include "render/model.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "math/matrix.h"
#include "render/light.h"
#include "game/stage/wall.h"
#include "math/vec2math.h"

//==================================================================================
// --- モデルの生成処理 ---
//==================================================================================
CModel *CModel::Create(const char *pXFileName,
	const Vector3 &pos, 
	const Vector3 &rot)
{
	CModel *pModel = new CModel;		// 生成したオブジェクトへのポインタ
	if (pModel != nullptr)
	{ // 生成に成功していれば、初期化処理
		pModel->Init(pXFileName, pos, rot);
	}

	return pModel;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CModel::CModel()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CModel::~CModel()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CModel::Init(const char *pXFileName, const Vector3 &pos, const Vector3 &rot)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = NULL;		// マテリアルへのポインタ
	HRESULT hr = S_OK;				// モデル読み込み結果

	// Xファイルの読み込み
	hr = D3DXLoadMeshFromX(pXFileName,			// 読み込むXファイル名
		D3DXMESH_SYSTEMMEM,
		pDevice,		// デバイスポインタ
		NULL,
		&m_pBuffMat,	// マテリアルへのポインタ
		NULL,
		&m_dwNumMat,	// マテリアルの数
		&m_pMesh);		// メッシュへのポインタ
	if (FAILED(hr))
	{ // 読み込み失敗
		return -1;
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

	// 引数の値を保存
	m_pos = pos;
	m_rot = rot;
	m_sFileName.append(pXFileName);

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CModel::Uninit(void)
{
	// メッシュを解放
	SafeRelease(m_pMesh);
	
	// マテリアルを解放
	SafeRelease(m_pBuffMat);

	// インデックスを破棄
	m_vIdx.clear();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CModel::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CModel::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = nullptr;	// マテリアルへのポインタ
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	Mtx::CalcWorld(&m_mtxWorld, m_pos, m_rot);

	if (m_pMtxParent != nullptr)
	{ // 親モデルが存在するなら、マトリックスを掛け合わせる
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, m_pMtxParent);
	}

	Matrix mtxShadow;		// シャドウマトリックス

// シャドウマトリックスを生成
	Mtx::CreateShadow(&m_mtxWorld,
		SHADOW_PLANE_POS,
		Vector3(0.0f, 1.0f, 0.0f),
		*pManager->GetLight()->GetLight(0),
		&mtxShadow);

	// シャドウマトリックスの設定
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
		pDevice->SetTexture(0, nullptr);

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	std::vector vpWall = CObject::FindObjectsByType(CObject::TYPE::WALL);
	for (auto &wall : vpWall)
	{
		CWall *pWall = static_cast<CWall *>(wall);
		Vector3 nor = VECTOR3_NULL;

		// 法線を計算
		Vector2 vec = Vec2::Direction(pWall->GetRotation()->y);
		nor.x = -vec.x;
		nor.z = -vec.y;

		// シャドウマトリックスを生成
		Mtx::CreateShadow(&m_mtxWorld,
			Vector3(pWall->GetPosition()->x + nor.x, pWall->GetPosition()->y, pWall->GetPosition()->z + nor.z),
			nor,
			*pManager->GetLight()->GetLight(0),
			&mtxShadow);

		// シャドウマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

		// 各マテリアルを描画
		for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
		{
			// マテリアルの設定
			D3DMATERIAL9 matShadow = pMat[nCntMat].MatD3D;			// マテリアルのコピー
			matShadow.Diffuse = Color(SHADOW_COLOR);		// マテリアルを黒色に変更

			pDevice->SetMaterial(&matShadow);

			// テクスチャの設定
			pDevice->SetTexture(0, nullptr);

			// モデル(パーツ)の描画
			m_pMesh->DrawSubset(nCntMat);
		}
	}

	// ワールドマトリックスの設定
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

		// テクスチャの設定 (指定テクスチャインデックスが無効出なければそちらを優先)
		pDevice->SetTexture(0, pTexture->GetAddress(m_vIdx[nCntMat]));

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}