//==================================================================================
// 
// 入力クラスのヘッダーファイル [input.h]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int NUM_KEY_MAX = 256;				// キーボードのキー数
constexpr int INPUT_DEFAULT_WAIT_PRESS = 60;	// リピートの連続入力までの待機時間
constexpr int INPUT_DEFAULT_INTERVAL = 30;		// リピートの連続入力の間隔

//**********************************************************************************
// *** 入力クラス ***
//**********************************************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(const HINSTANCE hInstance, const HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:
	inline static LPDIRECTINPUT8 m_pInput = nullptr;		// 入力オブジェクト
	LPDIRECTINPUTDEVICE8 m_pDevice = nullptr;				// 入力デバイス
};

//**********************************************************************************
// *** キーボードクラス ***
//**********************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(const HINSTANCE hInstance, const HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(const int nKey);
	bool GetTrigger(const int nKey);
	bool GetRelease(const int nKey);
	bool GetRepeat(const int nKey,
		const int nWaitPress = INPUT_DEFAULT_WAIT_PRESS, 
		const int nInterval = INPUT_DEFAULT_INTERVAL);

private:
	BYTE m_aKeyState[NUM_KEY_MAX] = {};				// キーボードのプレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX] = {};		// キーボードのトリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX] = {};		// キーボードのリリース情報
	int m_nKeyStateRepeat[NUM_KEY_MAX] = {};		// キーボードのリピート情報
};

//**********************************************************************************
// *** マウスクラス ***
//**********************************************************************************
class CInputMouse : public CInput
{
public:
	//******************************************************************************
	// *** マウスのボタンの種類 ***
	//******************************************************************************
	enum class KEY
	{
		LEFT = 0,		// 左クリック
		RIGHT,			// 右クリック
		WHEEL,			// 中クリック
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const KEY value) noexcept
	{
		return EnumUtil::InRange(value, KEY::LEFT, KEY::MAX);
	}

	CInputMouse();
	~CInputMouse();

	HRESULT Init(const HINSTANCE hInstance, const HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(const KEY key);
	bool GetTrigger(const KEY key);
	bool GetRelease(const KEY key);
	Vector3 GetPositionInScreen(void);
	Vector3 GetPositionInClient(void);

private:
	DIMOUSESTATE2 m_CurrentMouseState = {};		// マウスの入力情報
	DIMOUSESTATE2 m_PrevMouseState = {};		// マウスの過去の入力情報
	BYTE m_aMouseState[EnumUtil::ToIndex(KEY::MAX)] = {};			// マウスのプレス情報
	BYTE m_aMouseStateTrigger[EnumUtil::ToIndex(KEY::MAX)] = {};	// マウスのトリガー情報
	BYTE m_aMouseStateRelease[EnumUtil::ToIndex(KEY::MAX)] = {};	// マウスのリリース情報
};
#endif