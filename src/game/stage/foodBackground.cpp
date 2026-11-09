//==================================================================================
//
// 食べ物の背景演出クラスのソースファイル [foodBackground.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/stage/foodBackground.h"
#include "render/camera.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "render/xfile.h"
#include "math/rand.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float BACKGROUND_DISTANCE = 1800.0f;		// カメラからの距離
	constexpr float FAR_DISTANCE_RATIO = 0.8f;			// ファー距離に対する最大距離の割合
	constexpr float MODEL_SCREEN_HEIGHT = 0.22f;		// 画面の高さに対するモデルの基本の大きさ
	constexpr float SIZE_RATE_MIN = 0.55f;				// 大きさの倍率の最小値
	constexpr float SIZE_RATE_MAX = 1.8f;				// 大きさの倍率の最大値
	constexpr float POS_OFFSET_MAX = 0.4f;				// 配置位置のずれの最大値 (マス単位)
	constexpr float SCREEN_MARGIN = 1.1f;				// 画面端の隙間を防ぐための配置範囲の拡大率 (1.0で画面ぴったり)
	constexpr float ROTATION_SPEED = 0.006f;			// 回転速度 (ラジアン / フレーム)

	// 既存Xファイルの頂点範囲から求めた中心の高さ
	constexpr float MODEL_CENTER_Y[] = { 71.9156f, 40.5718f, 44.9440f };

	// 既存Xファイルの頂点範囲から求めた最大幅
	constexpr float MODEL_SIZE[] = { 177.90f, 224.8955f, 177.90f };

	// 食べ物モデルのファイルパス
	constexpr std::string_view MODEL_PATHS[] =
	{
		"data/MODEL/rice.x",
		"data/MODEL/humbarg.x",
		"data/MODEL/miso_soup.x"
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CFoodBackground *CFoodBackground::Create(void)
{
	CFoodBackground *pBackground = new CFoodBackground;		// 生成した背景
	if (FAILED(pBackground->Init()))
	{ // 生成失敗時はオブジェクト管理側で解放
		pBackground->Uninit();
		return nullptr;
	}

	return pBackground;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CFoodBackground::CFoodBackground(const int nPriority) : CObject(nPriority)
{ // タイプ指定 (食べ物アイテムとして検索されないようにする)
	SetType(TYPE::OBJ_X);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CFoodBackground::~CFoodBackground()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CFoodBackground::Init(void)
{
	// 定数配列の要素数はモデルの種類数と一致させる
	static_assert(std::size(MODEL_CENTER_Y) == FOOD_BACKGROUND_MODEL_COUNT);
	static_assert(std::size(MODEL_SIZE) == FOOD_BACKGROUND_MODEL_COUNT);
	static_assert(std::size(MODEL_PATHS) == FOOD_BACKGROUND_MODEL_COUNT);

	if (m_bReady || m_bEnded) return E_FAIL;		// 初期化済み

	CXFile *pXFile = CXFile::GetInstance();		// モデル管理

	for (int nCntModel = 0; nCntModel < FOOD_BACKGROUND_MODEL_COUNT; nCntModel++)
	{ // 食べ物モデルを登録
		m_aModelId[nCntModel] = pXFile->Register(MODEL_PATHS[nCntModel]);
		CXFile::CXInfo *pModel = pXFile->GetAddress(m_aModelId[nCntModel]);		// 登録したモデル

		// CXFile::Register は読み込み失敗時も番号を返すため、保存されたパスも確認する
		if (pModel == nullptr || pModel->GetFilePath() != MODEL_PATHS[nCntModel]) return E_FAIL;
	}

	CRand *pRand = CRand::GetInstance();		// 乱数生成インスタンス

	for (Decoration &decoration : m_aDecoration)
	{ // 種類・大きさ・位置・向きをばらつかせる
		decoration.nModel = pRand->Generate(0, FOOD_BACKGROUND_MODEL_COUNT - 1);
		decoration.fSizeRate = pRand->Generate(SIZE_RATE_MIN, SIZE_RATE_MAX);
		decoration.posOffset = Vector2(pRand->Generate(-POS_OFFSET_MAX, POS_OFFSET_MAX),
			pRand->Generate(-POS_OFFSET_MAX, POS_OFFSET_MAX));
		decoration.rotOffset = Vector3(pRand->Generate(-D3DX_PI, D3DX_PI),
			pRand->Generate(-D3DX_PI, D3DX_PI),
			pRand->Generate(-D3DX_PI, D3DX_PI));
	}

	m_bReady = true;
	Update();		// 最初の描画に備えて配置を計算

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CFoodBackground::Uninit(void)
{
	if (m_bEnded) return;		// 終了処理済み

	m_bEnded = true;
	m_bReady = false;

	CObject::Release();		// 管理リストからの解放を予約 (モデルは CXFile が管理する)
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CFoodBackground::Update(void)
{
	if (!m_bReady || m_bEnded) return;

	m_bPoseValid = false;		// カメラを取得できないフレームは描画しない

	// 描画対象のカメラを取得
	const CCamera::TYPE type = CCamera::GetFocus();		// 描画対象のカメラ
	if (!CCamera::IsValid(type)) return;

	CCamera *pCamera = CCamera::GetCamera(type);
	if (pCamera == nullptr) return;

	// カメラのビューマトリックスの逆行列を計算
	Matrix mtxView, mtxInverse;		// ビューマトリックスとその逆行列
	D3DXMatrixLookAtLH(&mtxView, pCamera->GetPosV(), pCamera->GetPosR(), pCamera->GetVecU());
	if (D3DXMatrixInverse(&mtxInverse, nullptr, &mtxView) == nullptr) return;

	const D3DVIEWPORT9 *pViewport = pCamera->GetViewport();		// ビューポート
	if (pViewport->Height == 0 || pCamera->GetFar() <= pCamera->GetNear()) return;

	// 配置する距離と、その距離での画面の大きさを計算
	const float fDistance = (std::min)(BACKGROUND_DISTANCE, pCamera->GetFar() * FAR_DISTANCE_RATIO);
	if (fDistance <= pCamera->GetNear()) return;

	const float fHalfHeight = fDistance * tanf(D3DXToRadian(pCamera->GetFovy()) * 0.5f);		// 画面の高さの半分
	const float fAspect = static_cast<float>(pViewport->Width) / static_cast<float>(pViewport->Height);		// アスペクト比
	const float fCellWidth = 2.0f * SCREEN_MARGIN / FOOD_BACKGROUND_COLUMN;		// 1マスの横幅 (画面上の相対値)
	const float fCellHeight = 2.0f * SCREEN_MARGIN / FOOD_BACKGROUND_ROW;		// 1マスの高さ (画面上の相対値)

	// 回転角を更新
	m_fAngle += ROTATION_SPEED;
	if (m_fAngle >= DOUBLE_PI) m_fAngle -= DOUBLE_PI;

	CXFile *pXFile = CXFile::GetInstance();		// モデル管理

	for (int nCntDecoration = 0; nCntDecoration < FOOD_BACKGROUND_COUNT; nCntDecoration++)
	{ // 各装飾のワールドマトリックスを計算
		Decoration &decoration = m_aDecoration[nCntDecoration];
		CXFile::CXInfo *pModel = pXFile->GetAddress(m_aModelId[decoration.nModel]);
		if (pModel == nullptr)
		{ // モデルが無くなっていれば描画しない
			m_bReady = false;
			return;
		}

		// 格子状に並べ、奇数行は半マスずらして隙間を目立たなくする
		const int nColumn = nCntDecoration % FOOD_BACKGROUND_COLUMN;	// 列番号
		const int nRow = nCntDecoration / FOOD_BACKGROUND_COLUMN;		// 行番号
		const float fShift = (nRow % 2 == 0) ? 0.0f : 0.5f;				// 行ごとの横ずれ (マス単位)
		const float fScreenX = -SCREEN_MARGIN + fCellWidth * (nColumn + 0.25f + fShift + decoration.posOffset.x);	// 画面上の相対位置X (-1 ～ 1)
		const float fScreenY = SCREEN_MARGIN - fCellHeight * (nRow + 0.5f + decoration.posOffset.y);				// 画面上の相対位置Y (-1 ～ 1)

		// 画面上の位置をワールド座標に変換
		const float fScale = (fHalfHeight * 2.0f * MODEL_SCREEN_HEIGHT * decoration.fSizeRate) / MODEL_SIZE[decoration.nModel];		// 拡大率
		const Vector3 posCamera(fScreenX * fHalfHeight * fAspect,
			fScreenY * fHalfHeight,
			fDistance);		// カメラ座標系での位置
		Vector3 posWorld;		// ワールド座標系での位置
		D3DXVec3TransformCoord(&posWorld, &posCamera, &mtxInverse);

		// モデル中心を揃え、ゆっくり回転させてから背景位置へ移動
		Matrix mtxCenter, mtxScale, mtxRot, mtxTrans;		// 計算用マトリックス
		D3DXMatrixTranslation(&mtxCenter, 0.0f, -MODEL_CENTER_Y[decoration.nModel], 0.0f);
		D3DXMatrixScaling(&mtxScale, fScale, fScale, fScale);
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			m_fAngle + decoration.rotOffset.y,
			decoration.rotOffset.x,
			decoration.rotOffset.z);
		D3DXMatrixTranslation(&mtxTrans, posWorld.x, posWorld.y, posWorld.z);

		decoration.mtxWorld = mtxCenter * mtxScale * mtxRot * mtxTrans;
	}

	m_bPoseValid = true;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CFoodBackground::Draw(void)
{
	if (!m_bReady || !m_bPoseValid || m_bEnded) return;

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイス
	CXFile *pXFile = CXFile::GetInstance();			// モデル管理
	CTexture *pTexture = CTexture::GetInstance();	// テクスチャ管理
	D3DMATERIAL9 matDef;		// 現在のマテリアル保存用

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	for (const Decoration &decoration : m_aDecoration)
	{ // 各装飾を描画
		CXFile::CXInfo *pModel = pXFile->GetAddress(m_aModelId[decoration.nModel]);
		if (pModel == nullptr) continue;

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &decoration.mtxWorld);

		for (int nCntMat = 0; nCntMat < pModel->GetMatrialNum(); nCntMat++)
		{ // パーツごとにマテリアルとテクスチャを設定して描画
			pDevice->SetMaterial(pModel->GetMaterial(nCntMat));
			pDevice->SetTexture(0, pTexture->GetAddress(pModel->GetTextureIdx(nCntMat)));
			pModel->DrawSubset(nCntMat);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	// 背景同士の前後関係は深度で正しく描き、ステージより手前に出ないよう深度だけクリア
	pDevice->Clear(0, nullptr, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
}