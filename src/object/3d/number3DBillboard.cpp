//==================================================================================
// 
// 数値3D表示(ビルボード)クラスのソースファイル [number3DBillboard.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/number3DBillboard.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "math/matrix.h"

//==================================================================================
// --- 数値オブジェクトの生成処理 (位置、サイズ、値指定) ---
//==================================================================================
CNumber3DBillboard *CNumber3DBillboard::Create(const Vector3 &pos,
	const Vector2 &size,
	const int nValue)
{
	CNumber3DBillboard *pNumber3D = new CNumber3DBillboard;		// 生成したオブジェクトへのポインタ
	if (pNumber3D != nullptr)
	{ // 生成に成功した場合、初期化
		pNumber3D->Init(pos, size, nValue);
	}

	return pNumber3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CNumber3DBillboard::CNumber3DBillboard()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CNumber3DBillboard::~CNumber3DBillboard()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CNumber3DBillboard::Init(const Vector3 &pos,
	const Vector2 &size,
	const int nNumber)
{
	// 初期化結果を返す
	return CNumber3D::Init(pos, size, nNumber);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CNumber3DBillboard::Uninit(void)
{ // 親クラスの終了
	CNumber3D::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CNumber3DBillboard::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CNumber3DBillboard::Draw(void)
{ // マトリックスを計算し、描画
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	Matrix mtxView;		// ビューマトリックス
	Matrix mtxWorld;	// ワールドマトリックス

	// カメラのビューマトリックスを取得 
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ワールドマトリックスを初期化
	Mtx::Identity(&mtxWorld);

	// ビューマトリックスを反転
	Mtx::Inverse(&mtxView, mtxView);

	// 逆行列によって入ってしまった位置情報を初期化
	mtxView._41 = 0.0f;
	mtxView._42 = 0.0f;
	mtxView._43 = 0.0f;

	// 位置の計算
	Mtx::CalcPosition(&mtxWorld, GetPosition());

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxView);

	const Matrix *pMtxParent = GetParent();		// 親マトリックスへのポインタ
	if (pMtxParent != nullptr)
	{ // 親が存在するならマトリックスを掛け合わせる
		Matrix mtxParentUnRotate;	// 回転を考慮しない親マトリックス

		// マトリックスを初期化
		D3DXMatrixIdentity(&mtxParentUnRotate);

		// 移動要素のみ取り出す
		mtxParentUnRotate._41 = pMtxParent->_41;
		mtxParentUnRotate._42 = pMtxParent->_42;
		mtxParentUnRotate._43 = pMtxParent->_43;

		// マトリックスを掛け合わせる
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxParentUnRotate);
	}

	// 描画
	CNumber3D::Draw(mtxWorld);
}