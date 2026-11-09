//==================================================================================
// 
// ライトクラスのソースファイル [camera.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "render/light.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "input/input.h"
#include "input/joypad.h"
#include "debug/debugproc.h"
#include "math/vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
//#define LIGHT_OPERABLE		// 一番ライトを操作可能か

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float DEFAULT_FOVY = 45.0f;	// 視野角
	constexpr float DEFAULT_ZN = 0.0f;		// 視野角
	constexpr float DEFAULT_ZF = 10000.0f;	// 視野角

	constexpr ConstColor LIGHT_DIFFUSE[DEFAULT_LIGHT_NUM] =		// 各ライトの拡散光
	{
		ConstColor(1.0f, 1.0f, 1.0f, 1.0f),
		ConstColor(0.7f, 0.7f, 0.7f, 1.0f),
		ConstColor(0.3f, 0.3f, 0.3f, 1.0f),
	};
	constexpr ConstVector3 LIGHT_DIRECTION[DEFAULT_LIGHT_NUM] =	// 各ライトの方向 (正規化前)
	{
		ConstVector3(0.0f, -0.5f, -0.4f),
		ConstVector3(-0.3f, 0.4f, 0.5f),
		ConstVector3(0.2f, 0.1f, 0.1f),
	};
	constexpr float LIGHT_ROT_SPEED = 0.05f;	// デバッグ操作時のライトの回転速度
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CLight::CLight()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CLight::~CLight()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CLight::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタ
	Vector3 vecDir[DEFAULT_LIGHT_NUM];			// ライトの方向ベクトル

	// ライトの情報を初期化
	memset(&m_aLight[0], 0, sizeof(m_aLight));

	// ライトの種類を設定
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;
	m_aLight[1].Type = D3DLIGHT_DIRECTIONAL;
	m_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライトの拡散光を設定
	m_aLight[0].Diffuse = Color(LIGHT_DIFFUSE[0]);
	m_aLight[1].Diffuse = Color(LIGHT_DIFFUSE[1]);
	m_aLight[2].Diffuse = Color(LIGHT_DIFFUSE[2]);

	// ライトの方向を設定
	vecDir[0] = LIGHT_DIRECTION[0];
	D3DXVec3Normalize(&vecDir[0], &vecDir[0]);
	m_aLight[0].Direction = vecDir[0];

	vecDir[1] = LIGHT_DIRECTION[1];
	D3DXVec3Normalize(&vecDir[1], &vecDir[1]);
	m_aLight[1].Direction = vecDir[1];

	vecDir[2] = LIGHT_DIRECTION[2];
	D3DXVec3Normalize(&vecDir[2], &vecDir[2]);
	m_aLight[2].Direction = vecDir[2];

	// ライトを設定する
	pDevice->SetLight(0, &m_aLight[0]);
	pDevice->SetLight(1, &m_aLight[1]);
	pDevice->SetLight(2, &m_aLight[2]);

	// ライトを有効にする
	pDevice->LightEnable(0, TRUE);
	pDevice->LightEnable(1, TRUE);
	pDevice->LightEnable(2, TRUE);

	// 操作するライトのインデックスを指定
	m_nIdxLight = 0;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CLight::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CLight::Update(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
#ifdef LIGHT_OPERABLE
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ
	auto pKeyboard = pManager->GetInputKeyboard();			// キーボードへのポインタ
	Vector3 vecDir = m_aLight[m_nIdxLight].Direction;	// ライトの向き 

	// 縦回転
	if (pKeyboard->GetPress(DIK_J))
	{
		m_rot.y -= LIGHT_ROT_SPEED;
	}
	else if (pKeyboard->GetPress(DIK_L))
	{
		m_rot.y += LIGHT_ROT_SPEED;
	}

	// 横回転
	if (pKeyboard->GetPress(DIK_I))
	{
		m_rot.z += LIGHT_ROT_SPEED;
	}
	else if (pKeyboard->GetPress(DIK_K))
	{
		m_rot.z -= LIGHT_ROT_SPEED;
	}

	// ベクトルを求める
	vecDir = Vec3::Arc(1.0f, m_rot.y, m_rot.z);

	// 正規化
	D3DXVec3Normalize(&vecDir, &vecDir);
	m_aLight[m_nIdxLight].Direction = vecDir;

	// ライトを再設定
	pDevice->SetLight(m_nIdxLight, &m_aLight[m_nIdxLight]);
#endif

#ifdef _DEBUG
	auto pProc = CManager::GetInstance()->GetDebugProc();		// デバッグ表示へのポインタ
	pProc->Print("====== ライト ======\n");
	pProc->Print("垂直回転 ({:.2f}) [I/K]\n", m_rot.z);
	pProc->Print("水平回転 ({:.2f}) [J/L]\n\n", m_rot.y);
#endif
}