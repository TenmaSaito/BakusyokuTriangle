//==================================================================================
// 
// インプットシステムクラスのソースファイル [inputSystem.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input/inputSystem.h"
#include "input/inputActionMap.h"
#include "input/inputControl.h"
#include "input/input.h"
#include "input/joypad.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInputSystem::CInputSystem()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInputSystem::~CInputSystem()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInputSystem::Init(const HINSTANCE hInstance,
    const HWND hWnd)
{
	HRESULT hr = S_OK;		// 処理結果

	// キーボードの生成
	if (m_pKeyboard == nullptr)
	{ // もしまだ生成されていないなら
		// キーボードを生成
		m_pKeyboard = std::make_unique<CInputKeyboard>();
		if (m_pKeyboard == nullptr)
		{ // 生成失敗の為、通知する
			MessageBox(hWnd, "キーボードの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// キーボードの初期化
		hr = m_pKeyboard->Init(hInstance, hWnd);
		if (FAILED(hr))
		{ // キーボードの初期化失敗の為、通知する
			MessageBox(hWnd, "キーボードの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// マウスの生成
	if (m_pMouse == nullptr)
	{ // もしまだ生成されていないなら
		// マウスを生成
		m_pMouse = std::make_unique<CInputMouse>();
		if (m_pMouse == nullptr)
		{ // 生成失敗の為、通知する
			MessageBox(hWnd, "マウスの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// マウスの初期化
		hr = m_pMouse->Init(hInstance, hWnd);
		if (FAILED(hr))
		{ // マウスの初期化失敗の為、通知する
			MessageBox(hWnd, "マウスの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// ジョイパッドの生成
	if (m_pJoypad == nullptr)
	{ // もしまだ生成されていないなら
		delete new int;
		// ジョイパッドを生成
		m_pJoypad = std::make_unique<CJoypad>();
		if (m_pJoypad == nullptr)
		{ // 生成失敗の為、通知する
			MessageBox(hWnd, "ジョイパッドの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// ジョイパッドの初期化
		m_pJoypad->Init();
	}

	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInputSystem::Uninit(void)
{
	for (auto &pActionMap : m_vpActionMap)
	{ // バッファ破棄
		SafeUniqueUninit(pActionMap);
	}

	// 配列クリア
	m_vpActionMap.clear();

	// ジョイパッドオブジェクトの破棄
	SafeUniqueUninit(m_pJoypad);

	// マウスオブジェクトの破棄
	SafeUniqueUninit(m_pMouse);

	// キーボードオブジェクトの破棄
	SafeUniqueUninit(m_pKeyboard);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CInputSystem::Update(void)
{
	// 各デバイスの更新
	if (m_pKeyboard != nullptr) m_pKeyboard->Update();
	if (m_pMouse != nullptr) m_pMouse->Update();
	if (m_pJoypad != nullptr) m_pJoypad->Update();

	for (auto &pActionMap : m_vpActionMap)
	{ // nullの場合スキップ
		if (pActionMap == nullptr) continue;

		// アクションマップの更新
		pActionMap->Update(*this);
	}
}

//==================================================================================
// --- アクションマップの作成処理 ---
//==================================================================================
CInputActionMap *CInputSystem::CreateActionMap(std::string_view sName)
{ // 名前がnullの場合アサート + スキップ
	assert(!sName.empty());
	if (sName.empty()) return nullptr;

	for (auto &pActionMap : m_vpActionMap)
	{ // nullの場合スキップ
		if (pActionMap == nullptr) continue;

		// 多重生成防止の為、同じ名前のマップが見つかればそれのポインタを返す
		if (pActionMap->IsEqualName(sName) == true) return pActionMap.get();
	}

	// アクションマップを生成
	auto &pActionMap = m_vpActionMap.emplace_back(std::make_unique<CInputActionMap>());

	// 初期化処理
	pActionMap->Init(sName);
	return pActionMap.get();
}

//==================================================================================
// --- アクションの検索処理 ---
//==================================================================================
CInputAction *CInputSystem::FindAction(std::string_view sMapName,
	std::string_view sActionName) const
{ // どちらかの名前が空ならアサート + スキップ
	assert(!sMapName.empty() && !sActionName.empty());
	if (sMapName.empty() || sActionName.empty()) return nullptr;

	CInputAction *pAction = nullptr;		// 検索したアクションへのポインタ

	for (auto &pActionMap : m_vpActionMap)
	{ // nullもしくは名前が違う場合スキップ
		if (pActionMap == nullptr) continue;
		else if (pActionMap->IsEqualName(sMapName) == false) continue;

		// アクションを検索
		pAction = pActionMap->Find(sActionName);
		if (pAction != nullptr) break;
	}

	return pAction;
}

//==================================================================================
// --- ボタン入力取得処理 ---
//==================================================================================
float CInputSystem::ReadControl(const INPUT_CONTROL_PATH &path) const
{ // デバイスタイプが無効もしくはデバイスインデックスが範囲外の場合、アサート + スキップ
	assert(IsValid(path.device));
	if (!IsValid(path.device)) return 0.0f;

	float fValue = 0.0f;		// 取得した値

	switch (path.device)
	{
	case INPUT_DEVICE::KEYBOARD:		// キーボード入力
		fValue = m_pKeyboard->GetPress(path.nControl) ? 1.0f : 0.0f;
		break;

	case INPUT_DEVICE::MOUSE:			// マウス入力
		fValue = m_pMouse->GetPress(EnumUtil::FromUnderlying<CInputMouse::KEY>(path.nControl)) ? 1.0f : 0.0f;
		break;

	case INPUT_DEVICE::JOYPAD_BUTTON:	// ジョイパッドのキー
		// デバイスインデックスチェック
		assert(CJoypad::IsValidPadID(path.nIdxDevice));
		if (!CJoypad::IsValidPadID(path.nIdxDevice)) return 0.0f;

		fValue = m_pJoypad->GetPress(static_cast<CJoypad::KEY>(path.nControl), path.nIdxDevice) ? 1.0f : 0.0f;
		break;

	case INPUT_DEVICE::JOYPAD_AXIS:	// ジョイパッドの軸指定スティック
		// デバイスインデックスチェック
		assert(CJoypad::IsValidPadID(path.nIdxDevice));
		if (!CJoypad::IsValidPadID(path.nIdxDevice)) return 0.0f;

		fValue = m_pJoypad->GetAxis(static_cast<CJoypad::AXIS>(path.nControl), path.nIdxDevice);
		break;

	default:
		break;
	}

	return fValue;
}

//==================================================================================
// --- スティック入力取得処理 ---
//==================================================================================
Vector3 CInputSystem::ReadStick(const INPUT_CONTROL_PATH &path) const
{ // デバイスタイプが無効もしくはデバイスインデックスが範囲外の場合、アサート + スキップ
	assert(path.device == INPUT_DEVICE::JOYPAD_STICK);
	assert(CJoypad::IsValidPadID(path.nIdxDevice));
	if (path.device != INPUT_DEVICE::JOYPAD_STICK
		|| !CJoypad::IsValidPadID(path.nIdxDevice)) return VECTOR3_NULL;

	Vector3 value = VECTOR3_NULL;	// 値
	m_pJoypad->GetStick(static_cast<CJoypad::STICK>(path.nControl), &value, path.nIdxDevice);

	return value;
}