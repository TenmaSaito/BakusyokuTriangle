//==================================================================================
// 
// プレイヤーカメラクラスのソースファイル [playerCamera.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/player/playerCamera.h"
#include "game/player/player.h"
#include "core/manager.h"
#include "input/input.h"
#include "debug/debugproc.h"
#include "math/vec2math.h"
#include "math/vec3math.h"
#include "util/util.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float LENGTH_PLAYER = 1500.0f;		// カメラとプレイヤーの距離
	constexpr float ROTATE_SPD = 0.04f;			// カメラの回転速度
	constexpr float POSITION_DEST_ACCELE = 0.03f;	// カメラの目標位置への移動速度
	constexpr float CAMERA_ROT_Z = -0.4f;		// カメラの見下ろし角度
}

//==================================================================================
// --- プレイヤーカメラ生成 ---
//==================================================================================
CPlayerCamera *CPlayerCamera::Create(CPlayer *pPlayer)
{
	CPlayerCamera *pPlayerCam = static_cast<CPlayerCamera*>(GetCamera(TYPE::PLAYER));		// プレイヤーカメラを取得
	if (pPlayerCam == nullptr)
	{ // nullで生成されていなければ、生成
		pPlayerCam = new CPlayerCamera;
	}

	// 生成出来ていれば各処理を実行
	if (pPlayerCam != nullptr)
	{ // 初期化
		pPlayerCam->Init(pPlayer);

		// フォーカスを設定
		pPlayerCam->SetFocus();
	}

	return pPlayerCam;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPlayerCamera::CPlayerCamera() : CCamera(TYPE::PLAYER)
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPlayerCamera::~CPlayerCamera()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CPlayerCamera::Init(CPlayer *pPlayer)
{ // 引数を保存
	m_pPlayer = pPlayer;

	Vector3 posV = *CCamera::GetPosV();		// 視点座標
	Vector3 posR = *CCamera::GetPosR();		// 注視点座標
	Vector3 rot = *CCamera::GetRotate();	// 角度

	// 角度を調整
	rot.z = CAMERA_ROT_Z;

	// プレイヤーの絶対座標を注視点に設定
	Vector3 posWorld;
	posR = *D3DXVec3TransformCoord(&posWorld, m_pPlayer->GetPosition(), m_pPlayer->GetMatrix());

	// 視点座標を注視点座標から求める
	posV.x = posR.x + (sinf(rot.z) * sinf(rot.y) * LENGTH_PLAYER);
	posV.y = posR.y + (cosf(rot.z) * LENGTH_PLAYER);
	posV.z = posR.z + (sinf(rot.z) * cosf(rot.y) * LENGTH_PLAYER);

	// 位置と角度を適用
	CCamera::Init(posV, posR);
	CCamera::SetRotate(rot);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPlayerCamera::Uninit(void)
{ // カメラの解放
	CCamera::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPlayerCamera::Update(void)
{
	Vector3 posV = *CCamera::GetPosV();		// 視点座標
	Vector3 posR = *CCamera::GetPosR();		// 注視点座標
	Vector3 rot = *CCamera::GetRotate();	// 角度
	Vector3 vecU = *CCamera::GetVecU();		// 上方向ベクトル
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ

#if 0
	// 視点回転
	if (pKeyboard->GetPress(DIK_Q))
	{
		rot.z -= ROTATE_SPD;
	}
	else if (pKeyboard->GetPress(DIK_E))
	{
		rot.z += ROTATE_SPD;
	}
	// 上方向ベクトル回転
	if (pKeyboard->GetPress(DIK_LEFT))
	{
		m_fPhi += ROTATE_SPD;
		m_fPhi = std::clamp(m_fPhi, -1.0f, 1.0f);
	}
	else if (pKeyboard->GetPress(DIK_RIGHT))
	{
		m_fPhi -= ROTATE_SPD;
		m_fPhi = std::clamp(m_fPhi, -1.0f, 1.0f);
	}
#endif

	// プレイヤーへのポインタが存在するなら
	if (m_pPlayer != nullptr)
	{ // プレイヤーへ滑らかに追従するために、過去の位置と現在位置で補間
		const Vector3 vecOrigin = VECTOR3_NULL;		// ローカル座標の原点
		Vector3 posWorld = *D3DXVec3TransformCoord(&posWorld, &vecOrigin, m_pPlayer->GetMatrix());
		Vector3 posDiff = posWorld - posR;				// 現在位置と予定位置の差分
		posR = posR + (posDiff * POSITION_DEST_ACCELE);		// 次の注視点座標を計算
	}

	// 視点座標を注視点座標から求める
	posV.x = posR.x + (sinf(rot.z) * sinf(rot.y) * LENGTH_PLAYER);
	posV.y = posR.y + (cosf(rot.z) * LENGTH_PLAYER);
	posV.z = posR.z + (sinf(rot.z) * cosf(rot.y) * LENGTH_PLAYER);

	// 位置と角度を適用
	CCamera::SetPosV(posV);
	CCamera::SetPosR(posR);
	CCamera::SetRotate(Vec3::FixedRotation(rot));
	CCamera::SetVecU(Vec2::ToVector3(Vec2::Arc(1.0f, m_fPhi * QUARTER_PI)));

//#ifdef _DEBUG
	// プレイヤーの位置を表示
	CDebugProc *pProc = CManager::GetInstance()->GetDebugProc();		// デバッグ表示へのポインタ
	pProc->ForcePrint("====== カメラ ======\n");
	pProc->ForcePrint("視点   ({:.2f}/{:.2f}/{:.2f}) [Q/E]\n", PRINT_VECTOR3(*GetPosV()));
	pProc->ForcePrint("注視点 ({:.2f}/{:.2f}/{:.2f})\n", PRINT_VECTOR3(*GetPosR()));
	pProc->ForcePrint("角度   ({:.2f}/{:.2f}/{:.2f})\n", PRINT_VECTOR3(*GetRotate()));
	pProc->ForcePrint("上方向 ({:.2f}/{:.2f}/{:.2f})\n\n", PRINT_VECTOR3(*GetVecU()));
//#endif
}

//==================================================================================
// --- 設置処理 ---
//==================================================================================
void CPlayerCamera::SetCamera(void)
{ // カメラの設置
	CCamera::SetCamera();
}