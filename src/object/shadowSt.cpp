//==================================================================================
// 
// ステンシルシャドウクラスのソースファイル [shadowSt.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/shadowSt.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/light.h"
#include "object/2d/polygon2D.h"
#include "math/vec3math.h"
#include "math/matrix.h"
#include "util/util.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view SHADOW_MODEL_PATH = "data/MODEL/shadowSt.x";		// 影に使うモデル
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CShadowSt *CShadowSt::Create(const Matrix *pTarget,
	std::string_view sModelPath)
{
	return Util::Create<CShadowSt>(pTarget, sModelPath);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CShadowSt::CShadowSt() : CObjectX(DEFAULT_SHADOW_PRIORITY)
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CShadowSt::~CShadowSt()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CShadowSt::Init(const Matrix *pTarget,
	std::string_view sModelPath)
{ // XFILEを読み込んだのち、2Dポリゴンの生成
	HRESULT hr = CObjectX::Init(sModelPath, VECTOR3_NULL, VECTOR3_NULL);
	if (FAILED(hr))
	{ // 失敗時中断
		return hr;
	}

	// ターゲットのマトリックスを親に設定
	CObjectX::SetParent(pTarget);

	// 画面全体を覆うポリゴンを作成
	m_pPolygon.reset(CPolygon2D::Create(SCREEN_MIDDLE, VECTOR3_NULL, SCREEN_SIZE));
	m_pPolygon->SetColor(Color(0.0f, 0.0f, 0.0f, 0.5f));
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CShadowSt::Uninit(void)
{ // バッファ解放
	SafeUniqueUninit(m_pPolygon);
	CObjectX::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CShadowSt::Update(const ManagerContext::RenderContext &context)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CShadowSt::Draw(const ManagerContext::RenderContext &context)
{
	LPDIRECT3DDEVICE9 pDev = context.pDevice;		// デバイス
	Vector3 posWorld = *D3DXVec3TransformCoord(&posWorld, &m_offset, GetParent());		// 絶対座標
	Matrix mtxWorld;

	// マトリックスを初期化
	Mtx::Identity(&mtxWorld);

	// マトリックスを計算
	Mtx::CalcScale(&mtxWorld, *GetScale());
	Mtx::CalcPosition(&mtxWorld, posWorld);

	// ステンシルテストを有効にする
	pDev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	
	// Zバッファへの書き込みを無効にする
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// カラーバッファへの書き込みを無効にする
	pDev->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);

	// ステンシルバッファの比較パラメータ設定
	pDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);
	pDev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT);
	pDev->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO);

	// 表面カリングに切り替える
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	CObjectX::Draw(mtxWorld, context);

	// ステンシルバッファの参照値を1に設定
	pDev->SetRenderState(D3DRS_STENCILREF, 0x01);

	// ステンシルバッファの比較パラメータ設定
	pDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);
	pDev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_ZERO);
	pDev->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO);

	// 裏面カリングに切り替える
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	CObjectX::Draw(mtxWorld, context);

	// カラーバッファへの書き込みを有効にする
	pDev->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// ステンシルバッファの参照値を2に設定
	pDev->SetRenderState(D3DRS_STENCILREF, 0x02);

	// ステンシルバッファの比較パラメータ設定
	pDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	pDev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	pDev->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);

	// ポリゴンの描画
	m_pPolygon->Draw();

	// Zバッファへの書き込みを有効にする
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ステンシルテストを無効にする
	pDev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}