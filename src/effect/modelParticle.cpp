//==================================================================================
//
// モデルパーティクルクラスのソースファイル [modelParticle.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "effect/modelParticle.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "render/texture.h"
#include "game/stage/stage.h"
#include "math/rand.h"
#include "math/vec3math.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int SHRINK_FRAME = 20;			// 消える前に縮んでいくフレーム数
	constexpr float FLOOR_FRICTION = 0.9f;		// 床に触れているときの横方向の減速率
	constexpr float STOP_BOUNCE_SPEED = 1.0f;	// これより遅い跳ね返りは止める
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CModelParticle *CModelParticle::Create(const Setting &setting)
{
	CModelParticle *pParticle = new CModelParticle;		// 生成したパーティクル
	if (FAILED(pParticle->Init(setting)))
	{ // 生成失敗時はオブジェクト管理側で解放
		pParticle->Uninit();
		return nullptr;
	}

	return pParticle;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CModelParticle::CModelParticle(const int nPriority) : CObject(nPriority)
{ // タイプ指定
	SetType(TYPE::MODEL_PARTICLE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CModelParticle::~CModelParticle()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CModelParticle::Init(const Setting &setting)
{
	CXFile *pXFile = CXFile::GetInstance();		// モデル管理
	CRand *pRand = CRand::GetInstance();		// 乱数生成インスタンス

	m_setting = setting;		// 設定を保存
	if (m_setting.nNum <= 0) return E_FAIL;		// 出現させる数が無い

	// モデルを登録
	m_uModelId = pXFile->Register(m_setting.sModelPath);
	CXFile::CXInfo *pModel = pXFile->GetAddress(m_uModelId);		// 登録したモデル

	// CXFile::Register は読み込み失敗時も番号を返すため、保存されたパスも確認する
	if (pModel == nullptr || pModel->GetFilePath() != m_setting.sModelPath) return E_FAIL;

	// 親にするステージを取得し、先に消えた場合に備えて監視
	m_pStage = CStage::Find();
	if (m_pStage != nullptr)
	{ // ステージがあれば監視開始
		m_pStage->AddObserver(this);
	}

	// モデル1個ごとの初期値を設定
	m_vPiece.resize(m_setting.nNum);
	for (Piece &piece : m_vPiece)
	{ // 位置・初速・大きさ・回転・寿命をばらつかせる
		piece.pos = m_setting.pos + Vec3::Random(-m_setting.posVariation * 0.5f, m_setting.posVariation * 0.5f);
		piece.move = m_setting.move + Vec3::Random(-m_setting.moveVariation * 0.5f, m_setting.moveVariation * 0.5f);
		piece.fScale = m_setting.fScale + pRand->Generate(-m_setting.fScaleVariation * 0.5f, m_setting.fScaleVariation * 0.5f);
		piece.rot = Vector3(pRand->Generate(-D3DX_PI, D3DX_PI),
			pRand->Generate(-D3DX_PI, D3DX_PI),
			pRand->Generate(-D3DX_PI, D3DX_PI));
		piece.rotMove = Vector3(pRand->Generate(-m_setting.fRotSpeedMax, m_setting.fRotSpeedMax),
			pRand->Generate(-m_setting.fRotSpeedMax, m_setting.fRotSpeedMax),
			pRand->Generate(-m_setting.fRotSpeedMax, m_setting.fRotSpeedMax));
		piece.nLife = m_setting.nLife + pRand->Generate(-m_setting.nLifeVariation, m_setting.nLifeVariation);

		if (piece.nLife < 1)
		{ // 寿命が無ければ1フレームだけ表示
			piece.nLife = 1;
		}
	}

	Update();		// 最初の描画に備えてマトリックスを計算

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CModelParticle::Uninit(void)
{
	if (m_pStage != nullptr)
	{ // ステージの監視を終了する
		m_pStage->RemoveObserver(this);
		m_pStage = nullptr;
	}

	// オブジェクト解放 (モデルは CXFile が管理する)
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CModelParticle::Update(void)
{
	if (IsDeath() == true) return;

	const Matrix *pMtxParent = GetStageMatrix();		// 親マトリックス
	Vector3 gravity(0.0f, -m_setting.fGravity, 0.0f);		// ステージ上のローカル座標での重力
	Vector2 sizeHalf = VECTOR2_NULL;		// ステージの大きさの半分 (床の範囲)

	if (pMtxParent != nullptr)
	{ // ワールドの下方向の重力を、傾いたステージのローカル座標に変換
		Matrix mtxInverse;		// 親マトリックスの逆行列
		if (D3DXMatrixInverse(&mtxInverse, nullptr, pMtxParent) != nullptr)
		{
			D3DXVec3TransformNormal(&gravity, &gravity, &mtxInverse);
		}

		sizeHalf = *m_pStage->GetSize() * 0.5f;
	}

	bool bAlive = false;		// 生きているモデルがあるか

	for (Piece &piece : m_vPiece)
	{ // 各モデルを更新
		if (piece.nLife <= 0) continue;

		piece.nLife--;
		bAlive = true;

		// 重力を加えて移動
		piece.move += gravity;
		piece.pos += piece.move;
		piece.rot += piece.rotMove;

		// ステージの床の上なら跳ね返す
		bool bOnFloor = (pMtxParent != nullptr)
			&& fabsf(piece.pos.x) <= sizeHalf.x
			&& fabsf(piece.pos.z) <= sizeHalf.y;		// 床の範囲内か
		if (bOnFloor == true && piece.pos.y < 0.0f && piece.move.y < 0.0f)
		{ // 床にめり込んだら、床の上に戻して跳ね返す
			piece.pos.y = 0.0f;
			piece.move.y = -piece.move.y * m_setting.fBounce;
			piece.move.x *= FLOOR_FRICTION;
			piece.move.z *= FLOOR_FRICTION;

			if (piece.move.y < STOP_BOUNCE_SPEED)
			{ // 弱い跳ね返りは止めて床に留まらせる
				piece.move.y = 0.0f;
			}
		}

		// 消える前は縮ませる
		float fScale = piece.fScale;		// 今回の大きさ
		if (piece.nLife < SHRINK_FRAME)
		{
			fScale *= static_cast<float>(piece.nLife) / static_cast<float>(SHRINK_FRAME);
		}

		// ワールドマトリックスを計算
		Matrix mtxScale, mtxRot, mtxTrans;		// 計算用マトリックス
		D3DXMatrixScaling(&mtxScale, fScale, fScale, fScale);
		D3DXMatrixRotationYawPitchRoll(&mtxRot, piece.rot.y, piece.rot.x, piece.rot.z);
		D3DXMatrixTranslation(&mtxTrans, piece.pos.x, piece.pos.y, piece.pos.z);
		piece.mtxWorld = mtxScale * mtxRot * mtxTrans;

		if (pMtxParent != nullptr)
		{ // ステージのマトリックスを掛け合わせる
			piece.mtxWorld *= *pMtxParent;
		}
	}

	if (bAlive == false)
	{ // 全てのモデルが消えたら終了
		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CModelParticle::Draw(void)
{
	if (IsDeath() == true) return;

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイス
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理
	CXFile::CXInfo *pModel = CXFile::GetInstance()->GetAddress(m_uModelId);		// 描画するモデル
	D3DMATERIAL9 matDef;		// 現在のマテリアル保存用

	if (pModel == nullptr) return;

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	for (const Piece &piece : m_vPiece)
	{ // 生きているモデルを描画
		if (piece.nLife <= 0) continue;

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &piece.mtxWorld);

		for (int nCntMat = 0; nCntMat < pModel->GetMatrialNum(); nCntMat++)
		{ // パーツごとにマテリアルとテクスチャを設定して描画
			pDevice->SetMaterial(pModel->GetMaterial(nCntMat));
			pDevice->SetTexture(0, pTexture->GetAddress(pModel->GetTextureIdx(nCntMat)));
			pModel->DrawSubset(nCntMat);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- 通知処理 ---
//==================================================================================
void CModelParticle::Notified(CObject *pObject, std::string_view message, const std::any &args)
{ // ポインタがステージへのポインタでは無かったら、スキップ
	if (pObject != m_pStage) return;
	// 死亡メッセージでは無かったならスキップ
	else if (message != NOTIFY_WHEN_DEATH) return;

	m_pStage = nullptr;		// ステージが死亡したため、ポインタをnullにする
}

//==================================================================================
// --- ステージのマトリックス取得処理 ---
//==================================================================================
const Matrix *CModelParticle::GetStageMatrix(void)
{
	if (m_pStage == nullptr) return nullptr;

	// 最新の傾きを反映したマトリックスを返す
	return m_pStage->GetMatrix();
}
