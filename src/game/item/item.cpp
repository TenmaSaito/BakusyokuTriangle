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
#include "core/serviceLocator.h"
#include "sound/sound.h"
#include "render/renderer.h"
#include "render/light.h"
#include "effect/particle.h"
#include "render/texture.h"
#include "ui/score.h"
#include "object/3d/objectBillboard3D.h"
#include "object/shadowSt.h"
#include "effect/itemBiteEffect.h"
#include "effect/modelParticle.h"
#include "object/3d/meshRing.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int ADD_SCORE = 10;		// スコアの加算値
#if defined(ENABLE_LOOP) || defined(ENABLE_ALL)
	constexpr std::string_view MODEL_PATHS[static_cast<UINT>(CItem::TYPE::MAX)] =		// 各アイテムのモデルパス
	{
		"",
		"data/MODEL/rice.x",
		"data/MODEL/humbarg.x",
		"data/MODEL/miso_soup.x",
	};
#elif defined(ENABLE_ORDER)
	constexpr std::string_view MODEL_PATHS[static_cast<UINT>(CItem::TYPE::MAX)] =		// 各アイテムのモデルパス
	{
		"",
		"data/MODEL/number/0.x",
		"data/MODEL/number/1.x",
		"data/MODEL/number/2.x",
		"data/MODEL/number/3.x",
		"data/MODEL/number/4.x",
		"data/MODEL/number/5.x",
		"data/MODEL/number/6.x",
		"data/MODEL/number/7.x",
		"data/MODEL/number/8.x",
		"data/MODEL/number/9.x",
	};
#endif

	const UINT PARTICLE_XFILE_IDS[static_cast<UINT>(CItem::TYPE::MAX)] =
	{
		IResourceable::INVALID_RESOURCE_ID,
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

#if defined(ENABLE_LOOP) || defined(ENABLE_ALL)
	// 影生成
	m_pShadowSt = CShadowSt::Create(GetMatrix());
	m_pShadowSt->SetScale(ConstVector3(10.0f, 10.0f, 10.0f));
	m_pShadowSt->SetOffset(ConstVector3(0.0f, 50.0f, 0.0f));
#endif

	// メッシュリング生成
	m_pRing = CMeshRing::Create(VECTOR3_NULL, VECTOR3_NULL, 100.0f, 50.0f, 10);
	m_pRing->SetParent(GetMatrix());
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CItem::Uninit(void)
{ // 紐づけられたオブジェクトのインスタンス破棄
	SafeUninit(m_pShadowSt);
	SafeUninit(m_pRing);

	// 親クラスの終了
	CObjectX::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CItem::Update(const ManagerContext::RenderContext &context)
{
	if (m_pPlayer == nullptr)
	{ // プレイヤーへのポインタが未取得の場合、取得
		m_pPlayer = CPlayer::Find();
	}

	if (m_pPlayer == nullptr)
	{ // プレイヤーがnullの場合スキップ
		return;
	}

	m_fRotate += 0.01f;
	Vector3 pos = *GetPosition();
	Vector3 rot = *GetRotation();
	pos.y += sinf(m_fRotate * 3.0f) * 0.5f;
	rot.y = m_fRotate;
	SetPosition(pos);
	SetRotation(rot);

	float fLength = Vec3::Length(*m_pPlayer->GetPosition(), *GetPosition());
	if (fLength <= m_fRadius + m_pPlayer->GetRadius())
	{ // 獲得半径内にいるかつプレイヤーが回転していれば、獲得ゲージを減らすため体力減少
		m_nLife--;
		
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
			m_pParticle->BindTexture(context.pTexture->Register("data/TEXTURE/effect000.jpg"));
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
		CServiceLocator::GetSound()->Play(CSound::LABEL::SE_BITE);

		CModelParticle::Setting setting = {};
		setting.pos = *GetPosition();
		setting.move = FOOD_PARTICLE_MOVE;
		setting.moveVariation = FOOD_PARTICLE_MOVE_VARIATION;
		setting.nLife = FOOD_PARTICLE_LIFE;
		setting.nNum = FOOD_PARTICLE_NUM;
		setting.fGravity = FOOD_PARTICLE_GRAVITY;
		setting.fBounce = FOOD_PARTICLE_BOUNCE;
#if defined(ENABLE_LOOP) || defined(ENABLE_ALL)
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
#elif defined(ENABLE_ORDER)
		setting.sModelPath += "rice.x";
#endif

		// パーティクル生成
		CModelParticle::Create(setting);

		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CItem::Draw(const ManagerContext::RenderContext &context)
{ 
	Matrix mtxShadow;		// シャドウマトリックス
	
	// 描画
	CObjectX::Draw(context);
}