//==================================================================================
// 
// 影クラスのソースファイル [shadow.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/shadow.h"
#include "core/manager.h"
#include "render/renderer.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CShadow *CShadow::Create(const Vector3 &pos,
	const Vector3 &rot,
	const Vector2 &size)
{
	CShadow *pShadow = new CShadow;		// 生成したオブジェクトへのポインタ
	if (pShadow != nullptr)
	{ // 初期化処理
		pShadow->Init(pos, rot, size);
	}

	return pShadow;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CShadow::CShadow(const int nPriority) : CObject3D(nPriority)
{ // タイプを指定
	SetType(TYPE::SHADOW);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CShadow::~CShadow()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CShadow::Init(const Vector3 &pos,
	const Vector3 &rot,
	const Vector2 &size)
{ // 親クラスの初期化
	return CObject3D::Init(pos, rot, size, false);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CShadow::Uninit(void)
{ // 親クラスの終了処理
	CObject3D::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CShadow::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CShadow::Draw(void)
{ // 影用に減算合成を掛ける
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 減算合成を有効にする
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	// 親クラスの描画処理
	CObject3D::Draw();

	// 減算合成を無効にする
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}