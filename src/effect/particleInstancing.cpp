//==================================================================================
//
// パーティクル(インスタンシング描画)クラスのソースファイル [particleInstancing.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "effect/particleInstancing.h"
#include "core/managerContext.h"
#include "core/serviceLocator.h"
#include "render/texture.h"
#include "render/shader.h"
#include "util/util.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CParticleInstancing *CParticleInstancing::Create(const Setting &setting, const int nMaxEffect)
{
	return Util::Create<CParticleInstancing>(setting, nMaxEffect);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CParticleInstancing::CParticleInstancing(const int nPriority) : CObject(nPriority)
{ // タイプ指定
	SetType(TYPE::PARTICLE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CParticleInstancing::~CParticleInstancing()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CParticleInstancing::Init(const Setting &setting, const int nMaxEffect)
{
	LPDIRECT3DDEVICE9 pDevice = CServiceLocator::GetDevice();		// デバイス
	HRESULT hr = S_OK;		// 処理結果

	// デバイスから、インスタンシングが使えるかを確認
	D3DCAPS9 caps;
	pDevice->GetDeviceCaps(&caps);
	if (caps.VertexShaderVersion < D3DVS_VERSION(3, 0))
	{ // インスタンシング描画不可
		return E_FAIL;
	}

	// 引数を保存
	m_setting = setting;	// 設定
	m_nMaxEffect = (nMaxEffect > MAX_INSTANCING) ? MAX_INSTANCING : nMaxEffect;		// エフェクト
	m_nMaxEffect = (m_nMaxEffect <= 0) ? 1 : m_nMaxEffect;		// 0防止

	// 配列サイズを事前に確保
	m_vInstData.reserve(static_cast<size_t>(m_nMaxEffect));

	// 共有頂点バッファを生成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_GEOMETRY) * 4,
		D3DUSAGE_WRITEONLY,
		NULL,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);
	if (FAILED(hr))
	{ // 生成失敗
		return hr;
	}

	VERTEX_GEOMETRY *pVtxGeometry = nullptr;		// 共有頂点情報へのポインタ

	// 共有頂点をロック
	hr = m_pVtxBuff->Lock(0, 0, reinterpret_cast<void**>(&pVtxGeometry), 0);
	if (SUCCEEDED(hr))
	{ // 共有頂点の設定
		pVtxGeometry[0].pos.x = -1.0f;
		pVtxGeometry[0].pos.y = 1.0f;
		pVtxGeometry[0].pos.z = 0.0f;

		pVtxGeometry[1].pos.x = 1.0f;
		pVtxGeometry[1].pos.y = 1.0f;
		pVtxGeometry[1].pos.z = 0.0f;

		pVtxGeometry[2].pos.x = -1.0f;
		pVtxGeometry[2].pos.y = -1.0f;
		pVtxGeometry[2].pos.z = 0.0f;

		pVtxGeometry[3].pos.x = 1.0f;
		pVtxGeometry[3].pos.y = -1.0f;
		pVtxGeometry[3].pos.z = 0.0f;

		pVtxGeometry[0].tex = Vector2(0.0f, 0.0f);
		pVtxGeometry[1].tex = Vector2(1.0f, 0.0f);
		pVtxGeometry[2].tex = Vector2(0.0f, 1.0f);
		pVtxGeometry[3].tex = Vector2(1.0f, 1.0f);

		// 共有頂点をアンロック
		m_pVtxBuff->Unlock();
	}
	else
	{ // ロック失敗
		return hr;
	}

	// インスタンス用バッファ生成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_INSTANCE) * m_nMaxEffect,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		NULL,
		D3DPOOL_DEFAULT,
		&m_pInstBuff,
		nullptr);
	if (FAILED(hr))
	{ // 生成失敗
		return hr;
	}

	// インデックスバッファ生成
	hr = pDevice->CreateIndexBuffer(sizeof(WORD) * 6,
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		nullptr);
	if (FAILED(hr))
	{ // 生成失敗
		return hr;
	}

	WORD *pIdx = nullptr;	// インデックスバッファへのポインタ

	// インデックスバッファをロック
	hr = m_pIdxBuff->Lock(0, 0, reinterpret_cast<void**>(&pIdx), 0);
	if (SUCCEEDED(hr))
	{ // インデックスバッファ設定
		pIdx[0] = 0;
		pIdx[1] = 1;
		pIdx[2] = 2;
		pIdx[3] = 2;
		pIdx[4] = 1;
		pIdx[5] = 3;

		// インデックスバッファをアンロック
		m_pIdxBuff->Unlock();
	}
	else
	{ // ロック失敗
		return hr;
	}

	D3DVERTEXELEMENT9 declElems[] =		// 頂点データレイアウト
	{
		// 変わらない頂点情報 : StreamSource 0
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },	// 頂点座標
		{ 0, sizeof(Vector3), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },	// テクスチャ座標

		// 変動する情報 : StreamSource 1
		{ 1,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},	// ワールド座標
		{ 1, sizeof(Vector3), D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },	// 半径
		{ 1, sizeof(Vector3) + sizeof(float), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },	// 色
		D3DDECL_END()
	};

	// 頂点データレイアウトを生成
	hr = pDevice->CreateVertexDeclaration(&declElems[0],
		&m_pVtxDeclBuff);
	if (FAILED(hr))
	{ // 生成失敗
		return hr;
	}

	CShader *pShader = CServiceLocator::GetShader();		// シェーダーへのポインタ

	// シェーダーを登録
	m_nIdxShader = pShader->Register("shader/instancing.hlsl");
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CParticleInstancing::Uninit(void)
{ // リソース破棄
	SafeRelease(m_pVtxBuff);
	SafeRelease(m_pInstBuff);
	SafeRelease(m_pIdxBuff);
	SafeRelease(m_pVtxDeclBuff);

	// 親クラスの終了
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CParticleInstancing::Update(const ManagerContext::RenderContext &context)
{
	size_t nCntInst = 0;	// インスタンスのカウンタ
	while (nCntInst < m_vInstData.size())
	{ // インスタンス数分繰り返す
		InstanceData &inst = m_vInstData[nCntInst];		// インスタンスデータへの参照

		// 更新
		inst.fRadius -= inst.fRadiusDecrease;
		inst.pos += inst.move;
		inst.nLife--;

		if (inst.fRadius <= 0.0f || inst.nLife <= 0)
		{ // 半径が0.0f以下、または体力が尽きたらSAP (末尾と入れ替えて削除)
			inst = m_vInstData.back();
			m_vInstData.pop_back();
			continue;	// 入れ替えた要素を同じ添え字で処理し直す
		}

		nCntInst++;
	}

	HRESULT hr = S_OK;		// 処理結果
	VERTEX_INSTANCE *pInst = nullptr;		// インスタンスバッファへのポインタ

	// null参照防止
	if (m_pInstBuff == nullptr) return;

	// インスタンスバッファをロック
	hr = m_pInstBuff->Lock(0, 0, reinterpret_cast<void **>(&pInst), D3DLOCK_DISCARD);
	if (FAILED(hr))
	{ // ロック失敗時、終了
		return;
	}

	for (int nCntInst = 0; nCntInst < m_vInstData.size(); nCntInst++)
	{ // インスタンスバッファを更新
		if (m_pMtxParent != nullptr)
		{ // 親が存在する場合、座標変換
			D3DXVec3TransformCoord(&pInst[nCntInst].pos, &m_vInstData[nCntInst].pos, m_pMtxParent);
		}
		else
		{ // 存在しないなら、そのまま配置
			pInst[nCntInst].pos = m_vInstData[nCntInst].pos;
		}
		pInst[nCntInst].fRadius = m_vInstData[nCntInst].fRadius;
		pInst[nCntInst].col = m_vInstData[nCntInst].col;
	}

	// インスタンスバッファをアンロック
	m_pInstBuff->Unlock();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CParticleInstancing::Draw(const ManagerContext::RenderContext &context)
{ // null参照防止
	if (m_pInstBuff == nullptr) return;

	LPDIRECT3DDEVICE9 pDevice = context.pDevice;		// デバイス
	LPD3DXEFFECT pInstancingShader = nullptr;	// インスタンシング用シェーダー
	CTexture *pTexture = context.pTexture;		// テクスチャへのポインタ
	CShader *pShader = context.pShader;			// シェーダーへのポインタ
	size_t numInst = m_vInstData.size();		// インスタンス数

	// 描画数が0以下ならスキップ
	if (numInst <= 0) return;

	// 頂点ストリームを指定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_GEOMETRY));	// 頂点バッファ
	pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(VERTEX_INSTANCE));	// インスタンスバッファ

	// インスタンスを宣言
	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numInst);
	pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	// 頂点データレイアウトを設定
	pDevice->SetVertexDeclaration(m_pVtxDeclBuff);

	// インデックスを設定
	pDevice->SetIndices(m_pIdxBuff);

	// シェーダーを取得
	pInstancingShader = pShader->GetAddress(m_nIdxShader);

	Matrix mtxView;		// ビューマトリックス
	Matrix mtxProj;		// プロジェクションマトリックス
	Matrix mtxViewProj;	// ビュープロジェクションマトリックス
	Vector3 camUp;		// カメラの上ベクトル
	Vector3 camRight;	// カメラの右ベクトル

	// ビューマトリックスを取得し、各方向ベクトルを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	camRight = Vector3(mtxView._11, mtxView._21, mtxView._31);
	camUp = Vector3(mtxView._12, mtxView._22, mtxView._32);

	// プロジェクションマトリックスを取得し、ビュープロジェクションマトリックスを作成
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);
	mtxViewProj = mtxView * mtxProj;

	// シェーダーにパラメータを設定
	pInstancingShader->SetTexture("g_texture", pTexture->GetAddress(m_nIdxTexture));		// テクスチャ
	pInstancingShader->SetFloatArray("g_vCamUp", camUp, 3);
	pInstancingShader->SetFloatArray("g_vCamRight", camRight, 3);
	pInstancingShader->SetMatrix("g_mViewProj", &mtxViewProj);

	// シェーダー開始
	UINT numPass;
	pInstancingShader->SetTechnique("Instancing");
	pInstancingShader->Begin(&numPass, 0);
	pInstancingShader->BeginPass(0);

	// ライティング無効
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	// 加算合成開始
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// インスタンシング描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,
		0,
		4,
		0,
		2);

	// 加算合成終了
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ライティングを有効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// シェーダー終了
	pInstancingShader->EndPass();
	pInstancingShader->End();

	// 頂点ストリームをクリーンアップ
	pDevice->SetStreamSourceFreq(0, 1);
	pDevice->SetStreamSourceFreq(1, 1);
}

//==================================================================================
// --- エフェクト生成処理 ---
//==================================================================================
void CParticleInstancing::Set(const Vector3 &pos,
	const Vector3 &move,
	const Color &col,
	const float fRadius,
	const float fRadiusDecrease,
	const int nLife)
{ // 出現させても表示されることが無い、もしくは既に最大数ならなら出現させない
	if (col.a <= 0.0f || fRadius <= 0.0f || nLife <= 0 || m_vInstData.size() >= m_nMaxEffect) return;

	// エフェクト生成
	m_vInstData.emplace_back(pos, move, col, fRadiusDecrease, fRadius, nLife);
}

//==================================================================================
// --- デバイスロスト時処理 ---
//==================================================================================
void CParticleInstancing::OnLostDevice(void)
{ // インスタンス用バッファ解放
	SafeRelease(m_pInstBuff);
}

//==================================================================================
// --- デバイスリセット時処理 ---
//==================================================================================
void CParticleInstancing::OnResetDevice(void)
{
	LPDIRECT3DDEVICE9 pDevice = CServiceLocator::GetDevice();		// デバイス

	// インスタンス用バッファ生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_INSTANCE) * m_nMaxEffect,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		NULL,
		D3DPOOL_DEFAULT,
		&m_pInstBuff,
		nullptr);
}