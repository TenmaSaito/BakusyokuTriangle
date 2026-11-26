//==================================================================================
//
// バネクラスのソースファイル [spring.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/spring.h"
#include "core/managerContext.h"
#include "core/serviceLocator.h"
#include "input/input.h"
#include "game/player/player.h"
#include "game/stage/stage.h"
#include "render/camera.h"
#include "math/vec3math.h"
#include "math/vec2math.h"
#include "util/util.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CSpring *CSpring::Create(const Vector3 &pos,
	const Vector3 &vec,
	const float fPower,
	const float fRadius)
{
	return Util::Create<CSpring>(pos, vec, fPower, fRadius);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSpring::CSpring(const int nPriority) : CObjectX(nPriority)
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSpring::~CSpring()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CSpring::Init(const Vector3 &pos,
	const Vector3 &vec,
	const float fPower,
	const float fRadius)
{ // 引数を保存
	m_vec = Vec3::Normalize(vec);
	m_fPower = fPower;
	m_fRadius = fRadius;
	
	// 初期化
	HRESULT hr = CObjectX::Init("data/MODEL/spring000.x", pos, Vec3::VectorToRotation(vec));
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CSpring::Uninit(void)
{ // 親クラスの終了
	CObjectX::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CSpring::Update(const ManagerContext::RenderContext &context)
{
	if (m_pPlayer == nullptr)
	{ // null参照防止の為、ポインタを検索
		CPlayer *pPlayer = CPlayer::Find();
		if (pPlayer == nullptr) return;

		m_pPlayer = pPlayer;		// ポインタ保存
	}

	if (m_pStage == nullptr)
	{ // null参照防止の為、ポインタを検索
		CStage *pStage = CStage::Find();
		if (pStage == nullptr) return;

		m_pStage = pStage;		// ポインタ保存
	}

	Vector3 posWorld = *GetWorldPosition(&posWorld);						// バネの絶対座標
	Vector3 posPlayerWorld = *m_pPlayer->GetWorldPosition(&posPlayerWorld);	// プレイヤーの絶対座標

	// 回転行列の各行 = ローカル軸のワールドでの向き（D3DX は行ベクトル）
	D3DXMATRIX mtxRot;
	Vector3 rot = *GetRotation();
	Vector3 pos = *GetPosition();
	Vector3 posPlayer = *m_pPlayer->GetPosition();
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);

	const Vector3 axis[3] =
	{
		Vector3(mtxRot._11, mtxRot._12, mtxRot._13),	// ローカル X
		Vector3(mtxRot._21, mtxRot._22, mtxRot._23),	// ローカル Y（= バネの向き）
		Vector3(mtxRot._31, mtxRot._32, mtxRot._33),	// ローカル Z
	};
	const float half[3] = { 10.0f, 10.6f, 10.0f };		// モデルのサイズの半分

	// 原点が底なので、中心はバネの向きに高さの半分ずらす
	const Vector3 center = pos + axis[1] * half[1];

	// OBB 上でプレイヤーにいちばん近い点を求める
	const Vector3 d = posPlayer - center;
	Vector3 closest = center;
	float local[3];
	for (int i = 0; i < 3; i++)
	{
		local[i] = D3DXVec3Dot(&d, &axis[i]);					// ローカル座標
		const float c = std::clamp(local[i], -half[i], half[i]);
		closest += axis[i] * c;
	}

	if (float fLength = Vec3::LengthSq(posPlayer, closest) <= m_fRadius * m_fRadius)
	{ // 当たっている場合は、法線ベクトルの向きに加速
		Vector2 vecXZ = Vec3::ToVector2(m_vec, Vec3::Axis::Y);
		m_pPlayer->AddForceUnreverse(Vec2::Direction(-vecXZ, VECTOR2_NULL), m_fPower * Vec2::Length(vecXZ), 5);
		m_pPlayer->AddForceUp(30.0f * m_vec.y);
		m_pStage->SetInvalidFrame(5);
	}

	CCamera *pFocus = CCamera::GetCamera(CCamera::GetFocus());
	CInputMouse *pMouse = CServiceLocator::GetInputMouse();
	Vector3 ray = pFocus->GetRayToScreen(pMouse->GetPositionInClient());
	if (IsHitByRay(*pFocus->GetPosV(), ray, 10000.0f)
		&& fabsf(ray.y) > 0.0001f)
	{
		if (pMouse->GetPress(CInputMouse::KEY::LEFT))
		{
			float t = -pFocus->GetPosV()->y / ray.y;
			if (t > 0.0f)
			{ // マウスで移動
				Vector3 pos = VECTOR3_NULL;
				pos.x = pFocus->GetPosV()->x + t * ray.x;
				pos.z = pFocus->GetPosV()->z + t * ray.z;
				SetPosition(pos);
			}
		}
		else if (pMouse->GetPress(CInputMouse::KEY::RIGHT))
		{
			float fValue = static_cast<float>(pMouse->GetMove(CInputMouse::SLOPE::LR)) / 100.0f;
			Vector3 rot = *GetRotation();
			rot.y += fValue;
			SetRotation(rot);
			m_vec = Vec3::Direction(rot);
		}
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CSpring::Draw(const ManagerContext::RenderContext &context)
{
	CObjectX::Draw(context);
}