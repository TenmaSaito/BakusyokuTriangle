//==================================================================================
// 
// プレイヤークラスのソースファイル [player.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/player/player.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "input/input.h"
#include "input/joypad.h"
#include "render/light.h"
#include "debug/debugproc.h"
#include "math/vec2math.h"
#include "math/vec3math.h"
#include "math/matrix.h"
#include "util/util.h"
#include "render/texture.h"
#include "render/camera.h"
#include "game/stage/stage.h"
#include "game/stage/wall.h"
#include "math/ray.h"
#include "object/3d/objectXAfterImage.h"
#include "object/3d/objectBillboard3D.h"
#include <bitset>

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float PLAYER_SPD = 1.0f;				// プレイヤーの移動速度
	constexpr float JUMP_FORCE = 7.0f;				// ジャンプの強さ
	constexpr float RESIST_POW = 0.25f;				// 摩擦
	constexpr float ANGLE_DEST_ACCELE = 0.1f;		// 目標角度への移動速度
	constexpr float MODEL_RADIUS = 10.0f;			// モデルの半径
	constexpr float VALUE_ROT = 0.005f;				// 回転速度
	constexpr float MAX_ROT_VALUE = 0.85f;			// 最大回転速度
	constexpr float RESIST_ROT = 0.017f;			// 減速速度
	constexpr float SCALE_VALUE = 1.0f;				// 拡大倍率
	constexpr float RAY_LENGTH = 50.0f;				// レイの長さ
	constexpr ConstVector3 VEC_QUAT = ConstVector3(1, 0, 0);		// 初期任意軸
	constexpr ConstColor VEC_PLAYER_RAY_COL = ConstColor(0.0f, 1.0f, 0.0f, 1.0f);		// プレイヤーの向きのレイの色
	constexpr ConstColor VEC_QUAT_RAY_COL = ConstColor(1.0f, 0.0f, 0.0f, 1.0f);		// 任意軸のレイの色
	constexpr std::string_view SHADOW_PATH = "data/TEXTURE/effect001.jpg";		// 影のテクスチャパス

	constexpr float DEFAULT_ANGLE_ADD = 0.1f;			// 自動回転時の角度の増加値
	constexpr float INSTANT_TURN_ANGLE = D3DX_PI * 0.75f;	// これ以上向きが変わると即時に振り向く角度
	constexpr float DIAGONAL_FRONT_ANGLE = D3DX_PI * 1.25f;	// 斜め前方向の入力時の角度 (カメラ基準)
	constexpr int AFTERIMAGE_LIFE = 15;				// 残像の寿命
	constexpr float AFTERIMAGE_START_ALPHA = 0.5f;		// 残像の開始α値
	constexpr float EAT_SCALE_RATE = 1.01f;			// アイテムを食べた時の拡大率
	constexpr float EAT_ROT_SPEED_RATE = 0.99f;		// アイテムを食べた時の回転速度の倍率
	constexpr float STICK_DEADZONE = 0.05f;				// 動いたと感知するデッドゾーン

	// デバッグ用ステータス変更
	constexpr int DEBUG_REPEAT_WAIT = 5;			// リピート入力の待機時間
	constexpr int DEBUG_REPEAT_INTERVAL = 1;		// リピート入力の間隔
	constexpr float DEBUG_SCALE_STEP = 0.1f;		// 拡大倍率の変更量
	constexpr float DEBUG_ROT_SPEED_STEP = 0.005f;	// 回転速度の変更量
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPlayer *CPlayer::Create(const std::string_view pFilename,
	const Vector3 &pos,
	const float fRadius)
{
	CPlayer *pPlayer = new CPlayer;		// 生成したオブジェクトへのポインタ
	if (pPlayer != nullptr)
	{ // 初期化処理
		pPlayer->Init(pFilename, pos, fRadius);
	}

	return pPlayer;
}

//==================================================================================
// --- 検索処理 ---
//==================================================================================
CPlayer *CPlayer::Find(void)
{
	return FindAnyObjectByType<CPlayer>(PLAYER_PRIORITY);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPlayer::CPlayer(const int nPriority) : CObjectX(nPriority)
{ // タイプの設定
	SetType(TYPE::PLAYER);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPlayer::~CPlayer()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPlayer::Init(const std::string_view pFilename,
	const Vector3 &pos,
	const float fRadius)
{ // 親クラスの初期化
	CObjectX::Init(pFilename, pos, VECTOR3_NULL);

	// 各メンバ変数を初期化
	m_bAutoRotating = false;
	m_fRadius = fRadius;
	m_vecAxis = VEC_QUAT;
	m_fValueRot = 0.0f;
	m_fRotSpeed = VALUE_ROT;
	m_fScaling = SCALE_VALUE;
	m_fAngleAdd = DEFAULT_ANGLE_ADD;
	Mtx::Identity(&m_mtxRot);

	// 吹き出し生成
	CObjectBillboard3D *pBill = CObjectBillboard3D::Create(ConstVector3(100.0f, 0.0f, 100.0f), 
		VECTOR3_NULL,
		ConstVector2(150.0f, 185.0f));
	pBill->SetParent(GetMatrix());
	pBill->BindTexture(CTexture::GetInstance()->Register("data/TEXTURE/next.png"));

	// レンダーステートの各種設定
	pBill->AddRenderState({ D3DRS_ZFUNC, D3DCMP_ALWAYS }, false);
	pBill->AddRenderState({ D3DRS_ZENABLE, FALSE }, false);
	pBill->AddRenderState({ D3DRS_ZWRITEENABLE, FALSE }, false);
	pBill->AddRenderState({ D3DRS_LIGHTING, FALSE }, false);

	pBill->AddRenderState({ D3DRS_ZFUNC, D3DCMP_LESSEQUAL }, true);
	pBill->AddRenderState({ D3DRS_ZENABLE, TRUE }, true);
	pBill->AddRenderState({ D3DRS_ZWRITEENABLE, TRUE }, true);
	pBill->AddRenderState({ D3DRS_LIGHTING, TRUE }, true);
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPlayer::Uninit(void)
{ // 親クラスの終了処理
	CObjectX::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPlayer::Update(void)
{
	if (m_pStage == nullptr)
	{ // ステージを取得していなかったら取得
		m_pStage = CStage::Find();

		// ステージの取得に失敗した場合はスキップ
		if (m_pStage == nullptr) return;
	}

	// 過去の座標を保存
	m_posOld = *GetPosition();

	// 自動回転フラグが立っていれば、傾いていないため角度を加算し回転
	if(m_bAutoRotating == true) AddForce(m_fAngle + m_fAngleAdd, m_fRotSpeed);
	if (m_fValueRot > MAX_ROT_VALUE)
	{ // 最大速度を超えていたら、修正
		m_fValueRot = MAX_ROT_VALUE;
	}
	else if (m_fValueRot > 0.0f)
	{ // 動いていない時は回転角を減少
		m_fValueRot -= RESIST_ROT * (m_fValueRot + (1.0f - m_fValueRot));
		if (m_fValueRot < 0.0f)
		{ // 0.0f以下になった場合は、0.0fに戻す
			m_fValueRot = 0.0f;
		}
	}

	// 回転軸を求める
	m_vecAxis = Vec2::ToVector3(Vec2::Direction(Util::FixedRotation(GetRotation()->y - HALF_PI)));
	m_vecAxis.z = m_vecAxis.y;
	m_vecAxis.y = 0.0f;

	// 入力処理を適用する為、移動量を計算
	float fCircumference = 0.0f;		// 円周の長さ
	float fPower = 0.0f;				// 移動量
	fCircumference = DOUBLE_PI * GetRadius();				// 円周の長さを求める
	fPower = (m_fValueRot / DOUBLE_PI) * fCircumference;	// 移動量を求める

	// 移動量を適用する為、加速度を更新
	Vector2 move = Vec2::Direction(Util::FixedRotation(-GetRotation()->y)) * fPower;
	m_move.x = move.x;
	m_move.z = -move.y;

	// 加速度を適用し、位置を更新
	SetPosition(*GetPosition() + m_move);
	if (GetPosition()->y < GetRadius())
	{ // 埋まっている為、座標を修正
		SetPosition(Vector3(GetPosition()->x, GetRadius(), GetPosition()->z));
	}

	// ステージから境界線を計算し、移動を制限
	Vector4 boader;		// 境界線
	Vector3 pos = *GetPosition();		// 現在位置

	// 境界線を計算
	boader.x = m_pStage->GetPosition()->x + (m_pStage->GetSize()->x * 0.5f) - GetRadius();		// 右端
	boader.y = m_pStage->GetPosition()->y + (m_pStage->GetSize()->y * 0.5f) - GetRadius();		// 上端
	boader.z = m_pStage->GetPosition()->y - (m_pStage->GetSize()->y * 0.5f) + GetRadius();		// 下端
	boader.w = m_pStage->GetPosition()->x - (m_pStage->GetSize()->x * 0.5f) + GetRadius();		// 左端

	// X軸の判定
	if (pos.x > boader.x) pos.x = boader.x;
	else if (pos.x < boader.w) pos.x = boader.w;

	// Z軸の判定
	if (pos.z > boader.y) pos.z = boader.y;
	else if (pos.z < boader.z) pos.z = boader.z;

	// 強制後の位置を反映
	SetPosition(pos);

	// 残像生成
	CObjectXAfterImage *pAfterImage = CObjectXAfterImage::Create(GetFileName(),
		*GetMatrix(),
		AFTERIMAGE_LIFE,
		AFTERIMAGE_START_ALPHA);

	pAfterImage->AddRenderState({ D3DRS_ZWRITEENABLE, FALSE }, false);
	pAfterImage->AddRenderState({ D3DRS_ZWRITEENABLE, TRUE }, true);

#ifdef _DEBUG
	// デバッグ用ライン表示
	CRay vecPlayer;		// プレイヤーの向き
	Vector2 vec = Vec2::Direction(Util::FixedRotation(-GetRotation()->y));		// プレイヤーの向き
	Vector3 posCalc = *D3DXVec3TransformCoord(&posCalc, GetPosition(), GetParent());

	// プレイヤーの向きの情報を指定し描画
	vecPlayer.SetStart(posCalc);
	vecPlayer.SetVector(Vector3(vec.x, 0.0f, -vec.y));
	vecPlayer.SetLength(RAY_LENGTH + GetRadius());
	vecPlayer.SetColor(VEC_PLAYER_RAY_COL);
	vecPlayer.Draw();

	// 任意軸の向きの情報を指定し描画
	vecPlayer.SetVector(m_vecAxis);
	vecPlayer.SetColor(VEC_QUAT_RAY_COL);
	vecPlayer.Draw();

	// 任意軸の逆向きの情報を指定し描画
	vecPlayer.SetVector(-m_vecAxis);
	vecPlayer.Draw();

	// デバッグ情報表示
	CDebugProc *pProc = CManager::GetInstance()->GetDebugProc();		// デバッグ表示へのポインタ
	pProc->Print("====== プレイヤー ======\n");
	pProc->Print("座標     ({:.2f}/{:.2f}/{:.2f})\n", PRINT_VECTOR3(*GetPosition()));
	pProc->Print("角度     ({:.2f}/{:.2f}/{:.2f})\n", PRINT_VECTOR3(*GetRotation()));
	pProc->Print("移動量   ({:.2f}/{:.2f}/{:.2f})\n", PRINT_VECTOR3(m_move));
	pProc->Print("回転軸   ({:.2f}/{:.2f}/{:.2f})\n", PRINT_VECTOR3(m_vecAxis));
	pProc->Print("回転量   ({:.4f})\n", m_fValueRot);
	pProc->Print("回転速度 ({:.4f})\n", m_fRotSpeed);
	pProc->Print("半径     ({:.4f}) [1/2]\n", GetRadius());
#endif
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPlayer::Draw(void)
{ // 親クラスの描画処理
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	Matrix *pMtx = GetMatrixPtr();		// マトリックスへのポインタ
	Matrix mtxWorld;		// ワールドマトリックス
	Matrix mtxShadow;		// シャドウマトリックス

	// マトリックスの初期化
	Mtx::Identity(&mtxWorld);

	// マトリックスのスケーリング
	Mtx::CalcScale(&mtxWorld, m_scl * m_fScaling);

	// 回転軸における指定の回転角からクォータニオンを作成
	D3DXQuaternionRotationAxis(&m_quat, &m_vecAxis, m_fValueRot);

	// クォータニオンから回転マトリックスを作成
	Mtx::CalcRotation(&m_mtxRot, m_quat);

	// 回転マトリックスを掛け合わせる
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &m_mtxRot);

	// 移動マトリックスを計算
	Vector3 posCalc = *D3DXVec3TransformCoord(&posCalc, GetPosition(), GetParent());
	Mtx::CalcPosition(&mtxWorld, posCalc);

	// 法線ベクトルの再正規化を有効化
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 描画
	CObjectX::Draw(mtxWorld);

	Vector3 nor = Vector3(0.0f, 1.0f, 0.0f);		// 床の法線

	// 法線を親マトリックスで計算
	D3DXVec3TransformNormal(&nor, &nor, GetParent());

	// 床のシャドウマトリックスを生成
	Mtx::CreateShadow(&mtxWorld,
		SHADOW_PLANE_POS,
		nor,
		*pManager->GetLight()->GetLight(0),
		&mtxShadow);

	// 影を描画
	CObjectX::DrawShadow(mtxShadow);
#if FALSE
	std::vector vpWall = CWall::FindAll();
	for (auto &pWall : vpWall)
	{
		Vector3 nor = VECTOR3_NULL;

		// 法線を計算
		Vector2 vec = Vec2::Direction(pWall->GetRotation()->y);
		nor.x = -vec.x;
		nor.z = -vec.y;

		// 壁のシャドウマトリックスを生成
		Mtx::CreateShadow(&mtxWorld,
			Vector3(pWall->GetPosition()->x + nor.x, pWall->GetPosition()->y, pWall->GetPosition()->z + nor.z),
			nor,
			*pManager->GetLight()->GetLight(0),
			&mtxShadow);

		// 親マトリックスと乗算
		Mtx::CalcParent(&mtxShadow, GetParent());

		// 影を描画
		CObjectX::DrawShadow(mtxShadow);
	}
#endif

	// 法線ベクトルの再正規化を無効化
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//==================================================================================
// --- 通知処理 ---
//==================================================================================
void CPlayer::Notified(CObject *pObject, std::string_view message, const std::any &args)
{
}

//==================================================================================
// --- 回転追加処理 ---
//==================================================================================
void CPlayer::AddForce(const float fRotDest, const float fValueRot)
{ // 回転角が負の値だとアサーション
	assert(fValueRot >= 0.0f);

	// 角度を保存
	m_rotDest.y = Util::FixedRotation(fRotDest);
	m_fAngle = m_rotDest.y;

	if (m_fValueRot <= FLT_MIN && m_fValueRot > -FLT_MIN)
	{ // 回転速度が0.0f以下の場合、角度を目標角度に強制
		SetRotation(Vector3(0.0f, Util::FixedRotation(m_rotDest.y), 0.0f));
	}

	m_fValueRot += fValueRot;

	// 差分を求める
	float fValue = m_rotDest.y - GetRotation()->y;

	// 差分を正規化して、絶対値に変換
	fValue = fabsf(Util::FixedRotation(fValue));
	if (fValue >= INSTANT_TURN_ANGLE)
	{ // プレイヤーの向きが前回とπ(+ε)分離れていたら、回転量をマイナスにし、角度即時適用
		m_fValueRot *= -1;

		// 角度更新
		SetRotation(Vector3(0.0f, m_rotDest.y, 0.0f));
	}
	else
	{ // 通常の角度更新
		float fAngle = m_rotDest.y - GetRotation()->y;
		fAngle = Util::FixedRotation(fAngle);

		SetRotation(Vector3(0.0f, Util::FixedRotation(GetRotation()->y + (fAngle * ANGLE_DEST_ACCELE)), 0.0f));
	}
}

//==================================================================================
// --- アイテム取得による拡大処理 ---
//==================================================================================
void CPlayer::Eat(void)
{
	m_fScaling *= EAT_SCALE_RATE;		// 拡大
	m_fRotSpeed *= EAT_ROT_SPEED_RATE;		// 速度は少し落とす
}

//==================================================================================
// --- 入力関連処理 ---
//==================================================================================
void CPlayer::CharactorMovable(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	Vector3 stick = VECTOR3_NULL;						// ジョイパッドのスティック入力
	float fRotDest = GetRotation()->y;					// 目標角度
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ
	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE::PLAYER);		// プレイヤーカメラへのポインタ
	const Vector3 *pCamRotate = pPlayerCam->GetRotate();				// カメラの角度

	if (pKeyboard->GetPress(DIK_W))
	{ // Wを押したとき
		if (pKeyboard->GetPress(DIK_A))
		{ // Aを押したとき
			// カメラの角度に合わせて、モデルの目標角度を求める！
			fRotDest = pCamRotate->y - DIAGONAL_FRONT_ANGLE;
			m_rotDest.y = Util::FixedRotation(fRotDest);
		}
		else if (pKeyboard->GetPress(DIK_D))
		{ // Dを押したとき
			// カメラの角度に合わせて、モデルの目標角度を求める！
			fRotDest = pCamRotate->y + DIAGONAL_FRONT_ANGLE;
			m_rotDest.y = Util::FixedRotation(fRotDest);
		}
		else
		{ // 純粋なW入力時
			// カメラの角度に合わせて、モデルの目標角度を求める！
			fRotDest = pCamRotate->y + D3DX_PI;
			m_rotDest.y = Util::FixedRotation(fRotDest);
		}

		if (m_fValueRot <= 0.0f)
		{ // 回転速度が0.0f以下の場合、角度を目標角度に強制
			SetRotation(Vector3(0.0f, Util::FixedRotation(m_rotDest.y), 0.0f));
		}

		m_fValueRot += m_fRotSpeed;		// 回転速度を更新
	}
	else if (pKeyboard->GetPress(DIK_S))
	{ // Sを押したとき
		if (pKeyboard->GetPress(DIK_A))
		{ // Aを押したとき
			// カメラの角度に合わせて、モデルの目標角度を求める！
			fRotDest = pCamRotate->y + QUARTER_PI;
			m_rotDest.y = Util::FixedRotation(fRotDest);
		}
		else if (pKeyboard->GetPress(DIK_D))
		{ // Dを押したとき
			// カメラの角度に合わせて、モデルの目標角度を求める！
			fRotDest = pCamRotate->y - QUARTER_PI;
			m_rotDest.y = Util::FixedRotation(fRotDest);
		}
		else
		{ // 純粋なS入力時
			// カメラの角度に合わせて、モデルの目標角度を求める！
			fRotDest = pCamRotate->y;
			m_rotDest.y = Util::FixedRotation(fRotDest);
		}

		if (m_fValueRot <= 0.0f)
		{ // 回転速度が0.0f以下の場合、角度を目標角度に強制
			SetRotation(Vector3(0.0f, Util::FixedRotation(m_rotDest.y), 0.0f));
		}

		m_fValueRot += m_fRotSpeed;		// 回転速度を更新
	}
	else if (pKeyboard->GetPress(DIK_A))
	{ // Aを押したとき
		// カメラの角度に合わせて、モデルの目標角度を求める！
		fRotDest = pCamRotate->y + HALF_PI;
		m_rotDest.y = Util::FixedRotation(fRotDest);
		if (m_fValueRot <= 0.0f)
		{ // 回転速度が0.0f以下の場合、角度を目標角度に強制
			SetRotation(Vector3(0.0f, Util::FixedRotation(m_rotDest.y), 0.0f));
		}

		m_fValueRot += m_fRotSpeed;		// 回転速度を更新
	}
	else if (pKeyboard->GetPress(DIK_D))
	{ // Dを押したとき
		// カメラの角度に合わせて、モデルの目標角度を求める！
		fRotDest = pCamRotate->y - HALF_PI;
		m_rotDest.y = Util::FixedRotation(fRotDest);
		if (m_fValueRot <= 0.0f)
		{ // 回転速度が0.0f以下の場合、角度を目標角度に強制
			SetRotation(Vector3(0.0f, Util::FixedRotation(m_rotDest.y), 0.0f));
		}

		m_fValueRot += m_fRotSpeed;		// 回転速度を更新
	}
	else if (pJoypad->GetStick(CJoypad::STICK::LEFT, &stick) && Vec3::Length(stick) > STICK_DEADZONE)
	{ // ジョイパッドからの入力で一定以上倒されていれば
		float fAngle = atan2f(stick.x, stick.y);		// スティックの倒された角度
		float fSpeed = Vec3::Length(stick);				// 倒された強さ

		// カメラの角度に合わせて、モデルの目標角度を求める！
		fRotDest = (fAngle + D3DX_PI);
		m_rotDest.y = Util::FixedRotation(fRotDest);
		if (m_fValueRot <= 0.0f)
		{ // 回転速度が0.0f以下の場合、角度を目標角度に強制
			SetRotation(Vector3(0.0f, Util::FixedRotation(m_rotDest.y), 0.0f));
		}

		m_fValueRot += m_fRotSpeed;		// 回転速度を更新
	}
	else if (m_fValueRot > 0.0f)
	{ // 動いていない時は回転角を減少
		m_fValueRot -= RESIST_ROT;
		if (m_fValueRot < 0.0f)
		{ // 0.0f以下になった場合は、0.0fに戻す
			m_fValueRot = 0.0f;
		}
	}

	// 角度修正
	fRotDest = Util::FixedRotation(fRotDest);

	// 差分を求める
	float fValue = fRotDest - GetRotation()->y;

	// 差分を正規化して、絶対値に変換
	fValue = fabsf(Util::FixedRotation(fValue));
	if (fValue >= INSTANT_TURN_ANGLE)
	{ // プレイヤーの向きが前回とπ(+ε)分離れていたら、回転量をマイナスにし、角度即時適用
		m_fValueRot *= -1;

		// 角度更新
		SetRotation(Vector3(0.0f, fRotDest, 0.0f));
	}
	else
	{ // 通常の角度更新
		float fAngle = m_rotDest.y - GetRotation()->y;
		fAngle = Util::FixedRotation(fAngle);

		SetRotation(Vector3(0.0f, Util::FixedRotation(GetRotation()->y + (fAngle * ANGLE_DEST_ACCELE)), 0.0f));
	}
}

//==================================================================================
// --- ステータス変更処理 ---
//==================================================================================
void CPlayer::StatusChange(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	const int nRepeatWait = DEBUG_REPEAT_WAIT;		// リピートの待機時間

	// 拡大倍率
	if (pKeyboard->GetRepeat(DIK_1, nRepeatWait, DEBUG_REPEAT_INTERVAL))
	{ // ↑を押したとき
		m_fScaling += DEBUG_SCALE_STEP;
	}
	else if (pKeyboard->GetRepeat(DIK_2, nRepeatWait, DEBUG_REPEAT_INTERVAL))
	{ // ↓を押したとき
		m_fScaling -= DEBUG_SCALE_STEP;
	}

	// 回転速度
	if (pKeyboard->GetRepeat(DIK_Y, nRepeatWait, DEBUG_REPEAT_INTERVAL))
	{ // ↑を押したとき
		m_fRotSpeed += DEBUG_ROT_SPEED_STEP;
	}
	else if (pKeyboard->GetRepeat(DIK_N, nRepeatWait, DEBUG_REPEAT_INTERVAL))
	{ // ↓を押したとき
		m_fRotSpeed -= DEBUG_ROT_SPEED_STEP;
	}
}