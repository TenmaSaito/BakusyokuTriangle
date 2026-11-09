//==================================================================================
// 
// ジョイパッドクラスのソースファイル [joypad.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/15
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input/joypad.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int BUTTON_BIT = 0x01;		// ボタン番号をビットに変換する基準
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CJoypad::CJoypad()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CJoypad::~CJoypad()
{
}

#pragma warning(push)
#pragma warning(disable : 4995)
//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CJoypad::Init(void)
{
	// XInputを有効にする
	XInputEnable(TRUE);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CJoypad::Uninit(void)
{ // バイブレーションを無効にする
	for (int nCntJoypad = 0; nCntJoypad < MAX_JOYPAD; nCntJoypad++) SetVibration(0, 0, 0, nCntJoypad);

	// XInputを無効にする
	XInputEnable(FALSE);
}
#pragma warning(pop)

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CJoypad::Update(void)
{
	XINPUT_STATE joykeyState;					// 入力情報
	XINPUT_INFO *pJoypad = &m_aJoypad[0];		// ジョイパッドの情報へのポインタ

	for (int nCntJoypad = 0; nCntJoypad < MAX_JOYPAD; nCntJoypad++, pJoypad++)
	{ // ジョイパッドの数分繰り返す
		if (XInputGetState(nCntJoypad, &joykeyState) == ERROR_SUCCESS)
		{ // ジョイパッドの状態取得成功時
			// 省略用変数
			XINPUT_STATE *pTrigger = &pJoypad->joykeyStateTrigger;		// トリガー情報
			XINPUT_STATE *pRelease = &pJoypad->joykeyStateRelease;		// リリース情報
			XINPUT_STATE *pPress = &pJoypad->joykeyState;				// プレス情報
			int *pRepeat = &pJoypad->nJoykeyStateRepeat[0];				// リピートカウント

			// トリガー情報を保存
			pTrigger->Gamepad.wButtons = ((joykeyState.Gamepad.wButtons ^ pPress->Gamepad.wButtons) & joykeyState.Gamepad.wButtons);

			// リリース情報を保存
			pRelease->Gamepad.wButtons = (pPress->Gamepad.wButtons & (pPress->Gamepad.wButtons ^ joykeyState.Gamepad.wButtons));
			
			// プレス情報を保存
			*pPress = joykeyState;		

			for (int nCntJoykey = 0; nCntJoykey < EnumUtil::ToUnderlying(KEY::MAX); nCntJoykey++)
			{ // ジョイパッドのボタン数分繰り返す
				if ((pPress->Gamepad.wButtons & (BUTTON_BIT << nCntJoykey)) == false)
				{ // ボタンが押されていなかった場合、リピートカウントリセット
					pRepeat[nCntJoykey] = 0;
				}
			}

			for (int nCntThumb = 0; nCntThumb < EnumUtil::ToUnderlying(STICK::MAX); nCntThumb++)
			{ // スティックの入力方向分繰り返す
				if (GetStick(static_cast<STICK>(nCntThumb)) == false)
				{ // 各スティックが倒されていなければ、リピートカウントリセット
					pJoypad->nJoythumbRepeat[nCntThumb] = 0;
				}
			}

			if (pJoypad->nCounterVibration > 0)
			{ // 振動カウントが0より大きい場合、減少させる
				pJoypad->nCounterVibration--;
				if (pJoypad->nCounterVibration <= 0)
				{ // 0以下になった場合、振動をストップ
					pJoypad->nCounterVibration = 0;
					pJoypad->vibration.wLeftMotorSpeed = 0;
					pJoypad->vibration.wRightMotorSpeed = 0;

					XInputSetState(0, &pJoypad->vibration);
				}
			}
		}
	}
}

//==================================================================================
// --- プレス判定処理 ---
//==================================================================================
bool CJoypad::GetPress(const KEY key, const int nPadID)
{
	if (!IsValid(key)) return false;		// 無効なキー
	return (m_aJoypad[nPadID].joykeyState.Gamepad.wButtons & (BUTTON_BIT << EnumUtil::ToIndex(key))) ? true : false;
}

//==================================================================================
// --- トリガー判定処理 ---
//==================================================================================
bool CJoypad::GetTrigger(const KEY key, const int nPadID)
{
	if (!IsValid(key)) return false;		// 無効なキー
	return (m_aJoypad[nPadID].joykeyStateTrigger.Gamepad.wButtons & (BUTTON_BIT << EnumUtil::ToIndex(key))) ? true : false;
}

//==================================================================================
// --- リリース判定処理 ---
//==================================================================================
bool CJoypad::GetRelease(const KEY key, const int nPadID)
{
	if (!IsValid(key)) return false;		// 無効なキー
	return (m_aJoypad[nPadID].joykeyStateRelease.Gamepad.wButtons & (BUTTON_BIT << EnumUtil::ToIndex(key))) ? true : false;
}

//==================================================================================
// --- リピート判定処理 ---
//==================================================================================
bool CJoypad::GetRepeat(const KEY key, const int nWaitPress, const int nInterval, const int nPadID)
{
	if (!IsValid(key)) return false;		// 無効なキー
	// リピートカウント増加
	m_aJoypad[nPadID].nJoykeyStateRepeat[EnumUtil::ToIndex(key)]++;
	if (m_aJoypad[nPadID].nJoykeyStateRepeat[EnumUtil::ToIndex(key)] <= nWaitPress)
	{ // リピートカウントが移行待機時間内なら、トリガーの判定を返す
		return (m_aJoypad[nPadID].joykeyStateTrigger.Gamepad.wButtons & (BUTTON_BIT << EnumUtil::ToIndex(key))) ? true : false;
	}
	else
	{ // リピートカウントがプレスへの移行待機時間を超えたなら
		if (m_aJoypad[nPadID].nJoykeyStateRepeat[EnumUtil::ToIndex(key)] % nInterval == 0)
		{ // インターバル分時間が経ったら、プレスの判定を返す
			return (m_aJoypad[nPadID].joykeyState.Gamepad.wButtons & (BUTTON_BIT << EnumUtil::ToIndex(key))) ? true : false;
		}
		else
		{ // インターバル中なら失敗
			return false;
		}
	}
}

//==================================================================================
// --- スティックの軸取得処理 ---
//==================================================================================
float CJoypad::GetAxis(const AXIS axis, const int nPadID)
{
	// 省略用変数
	XINPUT_GAMEPAD *pGamepad = &m_aJoypad[nPadID].joykeyState.Gamepad;		// ゲームパッドへのポインタ
	float fValue = 0.0f;	// 傾き度合い

	// 向き別で値を返す
	switch (axis)
	{
	case AXIS::LEFT_X:		// 左スティックのX軸
		fValue = static_cast<float>(pGamepad->sThumbLX) / static_cast<float>(SHRT_MAX);
		break;

	case AXIS::LEFT_Y:		// 左スティックのY軸
		fValue = static_cast<float>(pGamepad->sThumbLY) / static_cast<float>(SHRT_MAX);
		break;

	case AXIS::RIGHT_X:		// 右スティックのX軸
		fValue = static_cast<float>(pGamepad->sThumbRX) / static_cast<float>(SHRT_MAX);
		break;

	case AXIS::RIGHT_Y:		// 右スティックのY軸
		fValue = static_cast<float>(pGamepad->sThumbRY) / static_cast<float>(SHRT_MAX);
		break;

	default:
		break;
	}

	return fValue;
}

//==================================================================================
// --- スティック判定処理 ---
//==================================================================================
bool CJoypad::GetStick(const STICK stick, const float fRange, const int nPadID)
{
	// 省略用変数
	XINPUT_GAMEPAD *pGamepad = &m_aJoypad[nPadID].joykeyState.Gamepad;		// ゲームパッドへのポインタ
	short sRange = (short)(SHRT_MAX * fRange);		// shortへの変換後の範囲
	short sThumb;									// 判定する値

	if (stick >= STICK::RIGHT)
	{ // 右スティックの場合
		// 上下判定ならLY,左右判定ならLXの値を取得
		sThumb = (stick == STICK::RIGHT_UP || stick == STICK::RIGHT_DOWN) ? pGamepad->sThumbRY : pGamepad->sThumbRX;

		if (stick == STICK::RIGHT_UP || stick == STICK::RIGHT_RIGHT)
		{ // 上もしくは右に倒されているか判定したい場合
			return (sThumb >= sRange) ? true : false;
		}
		else
		{ // 下もしくは左に倒されているか判定したい場合
			return (sThumb <= -sRange) ? true : false;
		}
	}
	else
	{ // 左スティックの場合
		// 上下判定ならLY,左右判定ならLXの値を取得
		sThumb = (stick == STICK::LEFT_UP || stick == STICK::LEFT_DOWN) ? pGamepad->sThumbLY : pGamepad->sThumbLX;

		if (stick == STICK::LEFT_UP || stick == STICK::LEFT_RIGHT)
		{ // 上もしくは右に倒されているか判定したい場合
			return (sThumb >= sRange) ? true : false;
		}
		else
		{ // 下もしくは左に倒されているか判定したい場合
			return (sThumb <= -sRange) ? true : false;
		}
	}
}

//==================================================================================
// --- スティック取得処理 ---
//==================================================================================
bool CJoypad::GetStick(const STICK stick, Vector3 *pOut, const int nPadID)
{
	// 省略用変数
	XINPUT_GAMEPAD *pGamepad = &m_aJoypad[nPadID].joykeyState.Gamepad;		// ゲームパッドへのポインタ
	Vector3 out = Vector3(0, 0, 0);		// 出力する変換先変数

	// 出力先がNULLの場合、失敗
	if (pOut == NULL) return false;

	// 列挙型の指定が間違っている場合、失敗
	if (stick != STICK::RIGHT && stick != STICK::LEFT) return false;

	// スティックの値をVECTOR3に変換
	if (stick == STICK::RIGHT)
	{ // 右スティックの場合
		out.x = (float)pGamepad->sThumbRX / (float)SHRT_MAX;
		out.y = (float)pGamepad->sThumbRY / (float)SHRT_MAX;
	}
	else
	{ // 左スティックの場合
		out.x = (float)pGamepad->sThumbLX / (float)SHRT_MAX;
		out.y = (float)pGamepad->sThumbLY / (float)SHRT_MAX;
	}

	// 出力先へ書き出し
	*pOut = out;

	return true;
}

//==================================================================================
// --- スティック取得処理 (リピート) ---
//==================================================================================
bool CJoypad::GetStickRepeat(const STICK stick, 
	const float fRange, 
	const int nWaitPress,
	const int nInterval,
	const int nPadID)
{
	bool bInput = GetStick(stick, fRange, nPadID);					// スティックが倒されたかどうか
	int &rRepeat = m_aJoypad[nPadID].nJoythumbRepeat[EnumUtil::ToIndex(stick)];		// リピートカウントへの参照

	if (bInput == false) return false;		// 入力されていなかった場合、スキップ

	rRepeat++;
	if (rRepeat <= nWaitPress)
	{ // リピートカウントが移行待機時間内なら、トリガーの判定を返す
		return (rRepeat == 1) ? true : false;
	}
	else
	{ // リピートカウントがプレスへの移行待機時間を超えたなら
		if (rRepeat % nInterval == 0)
		{ // インターバル分時間が経ったら、プレスの判定を返す
			return bInput;
		}
		else
		{ // インターバル中なら失敗
			return false;
		}
	}
}

//==================================================================================
// --- 振動処理 ---
//==================================================================================
void CJoypad::SetVibration(const int nLPower,
	const int nRPower,
	const int nTime,
	const int nPadID)
{ // 引数を保存
	m_aJoypad[nPadID].nCounterVibration = nTime;
	m_aJoypad[nPadID].vibration.wLeftMotorSpeed = nLPower;
	m_aJoypad[nPadID].vibration.wRightMotorSpeed = nRPower;

	// バイブレーション設定
	XInputSetState(0, &m_aJoypad[nPadID].vibration);
}