//==================================================================================
//
// 口による画面遷移クラスのソースファイル [mouthTransition.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/transition/mouthTransition.h"
#include "object/2d/polygon2D.h"
#include "core/manager.h"
#include "render/texture.h"
#include <cmath>
#include <new>

//**********************************************************************************
// *** 定数・プロトタイプ宣言 ***
//**********************************************************************************
namespace
{
    // 開始と終了を滑らかにする補間
    float Smooth(float t) { return t * t * (3.0f - 2.0f * t); }

    constexpr std::string_view TASK_PATH = "data/TEXTURE/task.png";     // 牙テクスチャ
    constexpr ConstVector2 MOUTH_SIZE_RATE = ConstVector2(3.0f, 2.0f);	// 口ポリゴンの画面サイズに対する倍率
    constexpr float LOWER_UV_OFFSET = 0.5f;			// 下牙のUVのずらし量 (牙の重なり防止)
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMouthTransition::CMouthTransition()
{
    SetState(STATE::STAY);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMouthTransition::~CMouthTransition()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMouthTransition::Init(void)
{
    const Vector3 pos(0.0f, 0.0f, 0.0f);
    const Vector3 rot(0.0f, 0.0f, 0.0f);
    const Vector2 size(SCREEN_WIDTH * MOUTH_SIZE_RATE.x, SCREEN_HEIGHT * MOUTH_SIZE_RATE.y);

    // 上下の口を生成 (レンダラーの初期化後に呼び出すこと)
    m_pUpper.reset(CPolygon2D::Create(pos, rot, size));
    m_pLower.reset(CPolygon2D::Create(pos, rot, size));
    if (!m_pUpper || !m_pLower)
    {
        return E_FAIL;
    }

    // 上牙
    m_pUpper->SetColor(COLOR_ONE);
    m_pUpper->BindTexture(CTexture::GetInstance()->Register(TASK_PATH));

    // 下牙 (牙の重なりを抑えるため、UV座標をずらす)
    m_pLower->SetColor(COLOR_ONE);
    m_pLower->BindTexture(CTexture::GetInstance()->Register(TASK_PATH));
    m_pLower->SetUV({ Vector2(LOWER_UV_OFFSET, 0.0f), Vector2(1.0f + LOWER_UV_OFFSET, 0.0f), Vector2(LOWER_UV_OFFSET, 1.0f), Vector2(1.0f + LOWER_UV_OFFSET, 1.0f) });
    SetState(STATE::STAY);
    return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMouthTransition::Uninit(void)
{ // 各バッファの破棄
    SafeUniqueUninit(m_pUpper);
    SafeUniqueUninit(m_pLower);
    SetState(STATE::STAY);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMouthTransition::Update(void)
{
    if (GetState() == STATE::STAY || !m_pUpper || !m_pLower) return;
    m_elapsed++;

    float t = static_cast<float>(m_elapsed) / static_cast<float>(m_duration);
    if (t > 1.0f) t = 1.0f;
    if (GetState() == STATE::WIPE_IN)
    {
        ApplyPose(t);
        if (m_elapsed >= m_duration)
        { // 口が画面全体を覆ってからシーンを切り替える
            CManager::GetInstance()->SetMode(GetNextMode());
            EndTransition(m_duration);
        }
    }
    else
    { // 口の位置と角度の更新
        ApplyPose(1.0f - t);
        if (m_elapsed >= m_duration) SetState(STATE::STAY);
    }
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMouthTransition::Draw(void)
{
    if (GetState() == STATE::STAY || !m_pUpper || !m_pLower) return;
    m_pUpper->Draw();
    m_pLower->Draw();
}

//==================================================================================
// --- 遷移演出開始処理 ---
//==================================================================================
void CMouthTransition::StartTransition(int nTransitionTime, CScene::MODE modeNext)
{
    if (!m_pUpper || !m_pLower) return;
    m_duration = nTransitionTime > 0 ? nTransitionTime : 1;
    m_elapsed = 0;
    SetModeNext(modeNext);
    SetState(STATE::WIPE_IN);
    ApplyPose(0.0f);
}

//==================================================================================
// --- 遷移演出終了処理 ---
//==================================================================================
void CMouthTransition::EndTransition(int nTransitionTime)
{
    if (!m_pUpper || !m_pLower) return;
    m_duration = nTransitionTime > 0 ? nTransitionTime : 1;
    m_elapsed = 0;
    SetState(STATE::WIPE_OUT);
    ApplyPose(1.0f);
}

//==================================================================================
// --- 口の位置と角度の更新処理 ---
//==================================================================================
void CMouthTransition::ApplyPose(float progress)
{
    // 口を開いたまま右から進み、最後に閉じる
    const float advance = Smooth(progress < 0.72f ? progress / 0.72f : 1.0f);
    const float close = Smooth(progress > 0.72f ? (progress - 0.72f) / 0.28f : 0.0f);
    const float hingeX = SCREEN_WIDTH * (5.0f - 3.5f * advance);
    const float hingeY = SCREEN_HEIGHT * 0.5f;
    const float halfWidth = SCREEN_WIDTH * 1.5f;
    const float halfHeight = SCREEN_HEIGHT;
    const float angle = 0.20f * (1.0f - close);
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    // 閉じる際は歯の高さ以上に重ね、不透明な部分で隙間を覆う
    const float overlap = SCREEN_HEIGHT * 0.72f * close;

    // CPolygon2D は中心を軸に rotation.y で回転するため、中心位置も移動する
    m_pUpper->SetRotation(Vector3(0.0f, -angle, 0.0f));
    m_pUpper->SetPosition(Vector3(hingeX - halfWidth * c + halfHeight * s,
        hingeY - halfWidth * s - halfHeight * c + overlap, 0.0f));

    m_pLower->SetRotation(Vector3(0.0f, angle + D3DX_PI, 0.0f));
    m_pLower->SetPosition(Vector3(hingeX - halfWidth * c + halfHeight * s,
        hingeY + halfWidth * s + halfHeight * c - overlap, 0.0f));
}
