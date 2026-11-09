//==================================================================================
// 
// アイテムクラスのソースファイル [item.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/item.h"
#include "render/model.h"
#include "game/player/player.h"
#include "math/vec3math.h"
#include "math/matrix.h"
#include "core/manager.h"
#include "sound/sound.h"
#include "render/renderer.h"
#include "render/light.h"
#include "effect/particle.h"
#include "render/texture.h"
#include "ui/score.h"
#include "object/3d/objectBillboard3D.h"
#include "effect/itemBiteEffect.h"
#include "effect/modelParticle.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int ADD_SCORE = 10;		// スコアの加算値
	constexpr std::string_view MODEL_PATHS[static_cast<UINT>(CItem::TYPE::MAX)] =		// 各アイテムのモデルパス
	{
		"",
		"data/MODEL/rice.x",
		"data/MODEL/humbarg.x",
		"data/MODEL/miso_soup.x",
	};

	const UINT PARTICLE_XFILE_IDS[static_cast<UINT>(CItem::TYPE::MAX)] =
	{
		INVALID_XFILE_ID,
		CXFile::GetInstance()->PreRegister("data/MODEL/food_particle_rice.x"),
		CXFile::GetInstance()->PreRegister("data/MODEL/food_particle_humbarg.x"),
		CXFile::GetInstance()->PreRegister("data/MODEL/food_particle_miso_soup.x"),
	};

	// アイコン
	constexpr ConstVector3 ICON_OFFSET = ConstVector3(0.0f, 50.0f, 0.0f);	// アイコンの位置 (アイテムからの相対)
	constexpr ConstVector2 ICON_SIZE = VECTOR2_ONE * 50.0f;			// アイコンのサイズ

	// 食べている間のパーティクル
	constexpr ConstVector3 EAT_PARTICLE_MOVE = VECTOR3_ONE * 0.1f;			// 移動量
	constexpr ConstVector3 EAT_PARTICLE_MOVE_VARIATION = VECTOR3_ONE * 10.0f;	// 移動量のぶれ
	constexpr ConstVector2 EAT_PARTICLE_SCALE = ConstVector2(50.0f, 50.0f);		// 大きさ
	constexpr ConstColor EAT_PARTICLE_COLOR = ConstColor(1.0f, 1.0f, 1.0f, 0.5f);	// 色
	constexpr int EAT_PARTICLE_LIFE = 30;			// 体力
	constexpr int EAT_PARTICLE_NUM_PER_FRAME = 10;	// 1フレームの生成数

	// 食べ終わった時の演出
	constexpr float BITE_EFFECT_WIDTH = 160.0f;		// 噛みつきエフェクトの幅
	constexpr int BITE_EFFECT_TIME = 30;			// 噛みつきエフェクトの時間
	constexpr ConstVector3 FOOD_PARTICLE_MOVE = ConstVector3(0.0f, 8.0f, 0.0f);				// 食べかすの移動量
	constexpr ConstVector3 FOOD_PARTICLE_MOVE_VARIATION = ConstVector3(10.0f, 4.0f, 10.0f);	// 食べかすの移動量のぶれ
	constexpr int FOOD_PARTICLE_LIFE = 90;			// 食べかすの体力
	constexpr int FOOD_PARTICLE_NUM = 8;			// 食べかすの数
	constexpr float FOOD_PARTICLE_GRAVITY = 0.3f;	// 食べかすの重力
	constexpr float FOOD_PARTICLE_BOUNCE = 0.8f;	// 食べかすの反発係数
}

//==================================================================================
// --- アイテムの生成処理 ---
//==================================================================================
CItem *CItem::Create(const Vector3 &pos,
	const float fRadius,
	const int nLife,
	const TYPE type)
{
	CItem *pItem = new CItem;		// 生成したオブジェクトへのポインタ
	if (pItem != nullptr)
	{ // 生成出来ていれば初期化
		pItem->Init(pos, fRadius, nLife, type);
	}

	return pItem;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CItem::CItem(const int nPriority) : CObjectX(nPriority)
{ // タイプ指定
	SetType(CObject::TYPE::ITEM);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CItem::~CItem()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CItem::Init(const Vector3 &pos,
	const float fRadius,
	const int nLife,
	const TYPE type)
{ // モデル生成
	CObjectX::Init(MODEL_PATHS[static_cast<UINT>(type)], pos, VECTOR3_NULL);

	m_fRadius = fRadius;		// 半径保存
	m_nLife = nLife;			// 体力保存
	m_type = type;		// タイプ保存

	// 引数のタイプのアイコンを生成
	m_pIcon = CObjectBillboard3D::Create(ICON_OFFSET,
		VECTOR3_NULL,
		ICON_SIZE);

	// 親を登録
	m_pIcon->SetParent(GetMatrix());

	// 描画前設定を追加
	m_pIcon->AddRenderState({ D3DRS_ZFUNC, D3DCMP_ALWAYS }, false);
	m_pIcon->AddRenderState({ D3DRS_ZWRITEENABLE, FALSE }, false);
	m_pIcon->AddRenderState({ D3DRS_ZENABLE, FALSE }, false);

	// 描画後設定を追加
	m_pIcon->AddRenderState({ D3DRS_ZFUNC, D3DCMP_LESSEQUAL }, false);
	m_pIcon->AddRenderState({ D3DRS_ZWRITEENABLE, TRUE }, false);
	m_pIcon->AddRenderState({ D3DRS_ZENABLE, TRUE }, false);
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CItem::Uninit(void)
{ 
	if (m_pIcon)
	{ // アイコンを破棄
		m_pIcon->Uninit();
		m_pIcon = nullptr;
	}

	// 親クラスの終了
	CObjectX::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CItem::Update(void)
{
	if (m_pPlayer == nullptr)
	{ // プレイヤーへのポインタが未取得の場合、取得
		m_pPlayer = CPlayer::Find();
	}

	if (m_pScore == nullptr)
	{ // スコア表示へのポインタが未取得の場合、取得
		m_pScore = CScore::Find();
	}

	if (m_pPlayer == nullptr || m_pScore == nullptr)
	{ // プレイヤーがnullの場合スキップ
		return;
	}

	float fLength = Vec3::Length(*m_pPlayer->GetPosition(), *GetPosition());
	if (fLength <= m_fRadius + m_pPlayer->GetRadius())
	{ // 獲得半径内にいるかつプレイヤーが回転していれば、獲得ゲージを減らすため体力減少
		m_nLife--;
		m_pScore->AddScore(ADD_SCORE);
		
		if (m_pParticle == nullptr)
		{ // パーティクルを生成していないなら、新規生成
			CParticle::Setting setting = {};
			setting.pos = *GetPosition();
			setting.move = EAT_PARTICLE_MOVE;
			setting.scale = EAT_PARTICLE_SCALE;
			setting.color = EAT_PARTICLE_COLOR;
			setting.nLife = EAT_PARTICLE_LIFE;
			setting.nNumEffectFrame = EAT_PARTICLE_NUM_PER_FRAME;
			setting.moveVariation = EAT_PARTICLE_MOVE_VARIATION;
			setting.bEnableAlphaBlending = true;

			m_pParticle = CParticle::Create(setting);
			m_pParticle->BindTexture(CTexture::GetInstance()->Register("data/TEXTURE/effect000.jpg"));
		}

		// パーティクルを有効化
		m_pParticle->SetEnable(true);
	}
	else if (m_pParticle != nullptr)
	{ // パーティクルを無効化
		m_pParticle->SetEnable(false);
	}

	if (m_nLife <= 0)
	{ // 体力が尽きたら取得時処理を呼び出して死亡
		const Vector3 vecOrigin = VECTOR3_NULL;		// ローカル座標の原点
		Vector3 posWorld = *D3DXVec3TransformCoord(&posWorld, &vecOrigin, GetMatrix());
		m_pPlayer->Eat();
		CItemBiteEffect::Create(*GetPosition(), BITE_EFFECT_WIDTH, BITE_EFFECT_TIME, GetParent());
		CManager::GetInstance()->GetSound()->Play(CSound::LABEL::SE_BITE);

		CModelParticle::Setting setting = {};
		setting.pos = *GetPosition();
		setting.move = FOOD_PARTICLE_MOVE;
		setting.moveVariation = FOOD_PARTICLE_MOVE_VARIATION;
		setting.nLife = FOOD_PARTICLE_LIFE;
		setting.nNum = FOOD_PARTICLE_NUM;
		setting.fGravity = FOOD_PARTICLE_GRAVITY;
		setting.fBounce = FOOD_PARTICLE_BOUNCE;
		setting.sModelPath = "data/MODEL/food_particle_";
		
		// 種類によってパスを変える
		switch (m_type)
		{
		case CItem::TYPE::RICE:
			setting.sModelPath += "rice.x";
			break;
		case CItem::TYPE::SIDE_DISH:
			setting.sModelPath += "humbarg.x";
			break;
		case CItem::TYPE::SOUP:
			setting.sModelPath += "miso_soup.x";
			break;
		default:
			break;
		}

		// パーティクル生成
		CModelParticle::Create(setting);

		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CItem::Draw(void)
{ 
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	Matrix mtxShadow;		// シャドウマトリックス
	
	// 描画
	CObjectX::Draw();

	Vector3 nor = Vector3(0.0f, 1.0f, 0.0f);		// 床の法線

	// 法線を親マトリックスで計算
	D3DXVec3TransformNormal(&nor, &nor, GetParent());

	// 床のシャドウマトリックスを生成
	Mtx::CreateShadow(GetMatrix(),
		SHADOW_PLANE_POS,
		nor,
		*pManager->GetLight()->GetLight(0),
		&mtxShadow);

	// 影を描画
	CObjectX::DrawShadow(mtxShadow);
}