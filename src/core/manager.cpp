//==================================================================================
// 
// マネージャクラスのソースファイル [manager.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/12
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/manager.h"
#include "render/renderer.h"
#include "input/input.h"
#include "input/joypad.h"
#include "debug/debugproc.h"
#include "sound/sound.h"
#include "object/base/object.h"
#include "render/camera.h"
#include "game/player/playerCamera.h"
#include "render/light.h"
#include "render/texture.h"
#include "render/xfile.h"
#include "scene/transition/sceneTransition.h"
#include "ui/bitePause.h"
#include "util/util.h"
#include "math/matrix.h"
#include "game/player/player.h"
#include "ui/timer.h"
#include "math/vec2math.h"
#include "math/vec3math.h"
#include "input/inputSystem.h"
#include "input/inputActionMap.h"
#include "input/inputAction.h"
#include "input/inputBinding.h"
#include "input/inputControl.h"

// 定数宣言
namespace
{
	constexpr int TRANSITION_FRAME = CManager::SecToFrame(1);	// 遷移にかかる時間
	constexpr float MOVE_AXIS_PRESS_POINT = 0.1f;	// 移動軸の入力を押下とみなすしきい値
}

//==================================================================================
// --- マネージャの取得処理 ---
//==================================================================================
CManager *CManager::GetInstance(void)
{
	static CManager manager;		// インスタンス
	return &manager;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CManager::CManager()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CManager::~CManager()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CManager::Init(const HINSTANCE hInstance, const HWND hWnd, const BOOL bWindow)
{
	HRESULT hr = S_OK;		// 各関数の結果

	if (hWnd == nullptr)
	{ // ウィンドウハンドルがNULLの場合、失敗
		return E_FAIL;
	}

	// ウィンドウハンドルを保存
	m_hWnd = hWnd;

	// レンダラーの生成
	if (m_pRenderer == nullptr)
	{ // レンダラーがNULLの場合
		// レンダラーを生成
		m_pRenderer = std::make_unique<CRenderer>();
		if (m_pRenderer == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "レンダラーの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// レンダラーの初期化
		hr = m_pRenderer->Init(hWnd, bWindow);
		if (FAILED(hr))
		{ // レンダラーの初期化失敗
			MessageBox(hWnd, "レンダラーの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}
#ifndef ENABLE_INPUT_SYSTEM
	// キーボードの生成
	if (m_pInputKeyboard == nullptr)
	{ // もしまだ生成されていないなら
		// キーボードを生成
		m_pInputKeyboard = std::make_unique<CInputKeyboard>();
		if (m_pInputKeyboard == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "キーボードの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// キーボードの初期化
		hr = m_pInputKeyboard->Init(hInstance, hWnd);
		if (FAILED(hr))
		{ // キーボードの初期化失敗
			MessageBox(hWnd, "キーボードの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// マウスの生成
	if (m_pInputMouse == nullptr)
	{ // もしまだ生成されていないなら
		// マウスを生成
		m_pInputMouse = std::make_unique<CInputMouse>();
		if (m_pInputMouse == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "マウスの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// マウスの初期化
		hr = m_pInputMouse->Init(hInstance, hWnd);
		if (FAILED(hr))
		{ // マウスの初期化失敗
			MessageBox(hWnd, "マウスの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// ジョイパッドの生成
	if (m_pJoypad == nullptr)
	{ // もしまだ生成されていないなら
		// ジョイパッドを生成
		m_pJoypad = std::make_unique<CJoypad>();
		if (m_pJoypad == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "ジョイパッドの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// ジョイパッドの初期化
		m_pJoypad->Init();
	}

#else
	CInputSystem *pInputSystem = CInputSystem::GetInstance();	// インスタンス取得
	if (FAILED(pInputSystem->Init(hInstance, hWnd)))
	{ // 初期化失敗時、操作不可能な為exeを終了
		return E_FAIL;
	}

	// 横
	CInputActionMap *pMapLegacy = pInputSystem->CreateActionMap("Legacy");
	CInputAction *pHorizontal = pMapLegacy->Add("Horizontal", CInputAction::ACTION_TYPE::VALUE_1D, MOVE_AXIS_PRESS_POINT);

	// A / D
	pHorizontal->AddBinding(CInputBinding::CreateAxis1D(MakePathKeyboard(DIK_A), MakePathKeyboard(DIK_D)));

	// ← / →
	pHorizontal->AddBinding(CInputBinding::CreateAxis1D(MakePathKeyboard(DIK_LEFT), MakePathKeyboard(DIK_RIGHT)));

	// 左スティックX (符号付きの軸値をそのまま)
	pHorizontal->AddBinding(CInputBinding::CreateStick1D(MakePathJoypadAxis(EnumUtil::ToUnderlying(CJoypad::AXIS::LEFT_X))));

	// 縦
	CInputAction *pVertical = pMapLegacy->Add("Vertical", CInputAction::ACTION_TYPE::VALUE_1D, MOVE_AXIS_PRESS_POINT);

	// W / S
	pVertical->AddBinding(CInputBinding::CreateAxis1D(MakePathKeyboard(DIK_S), MakePathKeyboard(DIK_W)));

	// ↑ / ↓
	pVertical->AddBinding(CInputBinding::CreateAxis1D(MakePathKeyboard(DIK_DOWN), MakePathKeyboard(DIK_UP)));

	// 左スティックY (符号付きの軸値をそのまま)
	pVertical->AddBinding(CInputBinding::CreateStick1D(MakePathJoypadAxis(EnumUtil::ToUnderlying(CJoypad::AXIS::LEFT_Y))));
#endif

	// デバッグ表示の生成
	if (m_pDebugProc == nullptr)
	{ // もしまだ生成されていないなら
		// デバッグ表示を生成
		m_pDebugProc = std::make_unique<CDebugProc>();
		if (m_pDebugProc == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "デバッグ表示の生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// デバッグ表示の初期化
		hr = m_pDebugProc->Init(DEFAULT_SIZE, "PixelMplus12");
		if (FAILED(hr))
		{ // デバッグ表示の初期化失敗
			MessageBox(hWnd, "デバッグ表示の初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// 音声の生成
	if (m_pSound == nullptr)
	{ // もしまだ生成されていないなら
		// サウンドを生成
		m_pSound = std::make_unique<CSound>();
		if (m_pSound == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "サウンドの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// サウンドの初期化
		hr = m_pSound->Init(hWnd);
		if (FAILED(hr))
		{ // デバッグ表示の初期化失敗
			MessageBox(hWnd, "サウンドの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// テクスチャの読み込み
	CTexture::GetInstance()->Load();

	// モデルの読み込み
	CXFile::GetInstance()->Load();

	// ライトの生成
	m_pLight = std::make_unique<CLight>();
	m_pLight->Init();

	// 遷移演出の作成
	m_pTransition.reset(CSceneTransition::Create(CSceneTransition::TYPE::MOUTH));

	// ポーズ画面の作成
	m_pPause.reset(CBitePause::Create());

	// モード作成
	SetMode(CScene::MODE::TITLE);

	// 成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CManager::Uninit(void)
{
	// 全オブジェクトの破棄, 終了処理
	CObject::ReleaseAll();

	// 全カメラの破棄、終了処理
	CCamera::ReleaseAll();

	// XFILEの破棄
	CXFile::GetInstance()->Unload();

	// テクスチャの破棄
	CTexture::GetInstance()->Unload();

	// ポーズオブジェクトの破棄
	SafeUniqueUninit(m_pPause);

	// 遷移演出の破棄
	SafeUniqueUninit(m_pTransition);

	// ライトオブジェクトの破棄
	SafeUniqueUninit(m_pLight);

	// サウンドオブジェクトの破棄
	SafeUniqueUninit(m_pSound);

	// デバッグ表示オブジェクトの破棄
	SafeUniqueUninit(m_pDebugProc);

#ifndef ENABLE_INPUT_SYSTEM
	// ジョイパッドオブジェクトの破棄
	SafeUniqueUninit(m_pJoypad);

	// マウスオブジェクトの破棄
	SafeUniqueUninit(m_pInputMouse);

	// キーボードオブジェクトの破棄
	SafeUniqueUninit(m_pInputKeyboard);
#else
	// インプットシステムの終了
	CInputSystem::GetInstance()->Uninit();
#endif

	// レンダラーオブジェクトの破棄
	SafeUniqueUninit(m_pRenderer);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CManager::Update(void)
{
#ifndef ENABLE_INPUT_SYSTEM
	// キーボードの更新処理
	m_pInputKeyboard->Update();

	// マウスの更新処理
	m_pInputMouse->Update();

	// ジョイパッドの更新処理
	m_pJoypad->Update();
#else
	// インプットシステムの更新
	CInputSystem::GetInstance()->Update();
#endif

	// デバッグ表示の更新処理
	m_pDebugProc->Update();
	
#ifdef _DEBUG
	// デバッグ情報
	m_pDebugProc->Print("======== デバッグ情報 ========\n");
	m_pDebugProc->Print("FPS : ({})\n", m_nCountFPS);
	m_pDebugProc->Print("リロード : ({}) [BackSpace]\n\n", m_nCountReload);
#endif
	// 操作説明
	m_pDebugProc->ForcePrint("======== 操作 ========\n");
	m_pDebugProc->ForcePrint("WASD : 台を傾ける\n");
	m_pDebugProc->ForcePrint("BACK : リトライ\n");

	// ライトの更新処理
	m_pLight->Update();

	if (GetInputKeyboard()->GetTrigger(DIK_BACK))
	{ // リセット
		SetTransition(CScene::MODE::RANKING);
		m_nCountReload++;
	}

	// ポーズの更新
	m_pPause->Update();

	if (m_pPause->GetDisp() != true)
	{
		// カメラの更新
		CCamera::UpdateAll();

		// レンダラーの更新処理
		m_pRenderer->Update();
	}
	else
	{ // ポーズ中なら、デバッグ表示
		m_pDebugProc->Print("[現在ポーズ中！]\n");
	}

	ManagerContext::RenderContext renderContext;
	ManagerContext::SceneContext sceneContext = { .TRANSITION_FRAME = TRANSITION_FRAME };			// シーンコンテキスト

	// 描画コンテキストを作成
	renderContext.pDevice = m_pRenderer->GetDevice();
	renderContext.pFocus = CCamera::GetCamera(CCamera::GetFocus());
	renderContext.pLight = m_pLight.get();
	renderContext.pTexture = CTexture::GetInstance();
	renderContext.pXFile = CXFile::GetInstance();

	// シーンコンテキストを作成
	sceneContext.pRenderContext = &renderContext;
	sceneContext.pInputSystem = CInputSystem::GetInstance();
	sceneContext.pSound = m_pSound.get();
	sceneContext.pTransition = m_pTransition.get();

	// シーンの更新
	m_pScene->Update(sceneContext);

	// トランジション更新
	m_pTransition->Update();

	// フレームカウンターを増加
	m_nCounterFrame++;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CManager::Draw(void)
{ // レンダラーの描画
	m_pRenderer->Draw();
}

//==================================================================================
// --- ポーズ状態設定処理 ---
//==================================================================================
void CManager::SetEnablePause(const bool bEnable)
{
	m_pPause->SetDisp(bEnable);
}

//==================================================================================
// --- ポーズ状態取得処理 ---
//==================================================================================
bool CManager::GetEnablePause(void)
{
	return m_pPause->GetDisp();
}

//==================================================================================
// --- モード変更処理 ---
//==================================================================================
HRESULT CManager::SetMode(const CScene::MODE modeNext)
{
	// 全音源の再生の停止
	m_pSound->Stop();

	if (m_pScene != nullptr)
	{ // シーンが生成されていれば終了処理
		m_pScene->Uninit();
		m_pScene.reset();
	}

	// 全オブジェクトの破棄
	CObject::ReleaseAll();

	// カメラの破棄
	CCamera::ReleaseAll();

	// ポーズの解除
	m_bPause = false;

	ManagerContext::RenderContext renderContext;		// 描画コンテキスト
	ManagerContext::SceneContext sceneContext;			// シーンコンテキスト

	// 描画コンテキストを作成
	renderContext.pDevice = m_pRenderer->GetDevice();
	renderContext.pFocus = CCamera::GetCamera(CCamera::GetFocus());
	renderContext.pLight = m_pLight.get();
	renderContext.pTexture = CTexture::GetInstance();
	renderContext.pXFile = CXFile::GetInstance();

	// シーンコンテキストを作成
	sceneContext.pRenderContext = &renderContext;
	sceneContext.pInputSystem = CInputSystem::GetInstance();
	sceneContext.pSound = m_pSound.get();
	sceneContext.pTransition = m_pTransition.get();

	// シーンの生成
	CScene::Create(sceneContext, modeNext, m_pScene);
	m_pPause->SetDisp(false);
	return (m_pScene == nullptr) ? E_FAIL : S_OK;
}

//==================================================================================
// --- トランジション設定処理 ---
//==================================================================================
void CManager::SetTransition(const CScene::MODE modeNext)
{
	m_pTransition->StartTransition(TRANSITION_FRAME, modeNext);
}

//==================================================================================
// --- トランジション確認処理 ---
//==================================================================================
bool CManager::IsTransition(void) const
{
	return m_pTransition->GetState() != CSceneTransition::STATE::STAY;
}