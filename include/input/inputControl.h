//==================================================================================
// 
// 入力操作の型をまとめたヘッダーファイル [inputControl.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _INPUT_CONTROL_H_		// インクルードガード
#define _INPUT_CONTROL_H_
#include "util/enumUtil.h"

//**********************************************************************************
// *** 入力受付の対象デバイス ***
//**********************************************************************************
enum class INPUT_DEVICE
{
	NONE = 0,			// 無し
	KEYBOARD,			// キーボード
	MOUSE,				// マウス
	JOYPAD_BUTTON,		// ジョイキー
	JOYPAD_STICK,		// ジョイスティック
	JOYPAD_AXIS,		// ジョイパッド軸指定
	MAX
};

// 有効値かどうかの判定
constexpr bool IsValid(const INPUT_DEVICE value) noexcept
{
	return EnumUtil::InRange(value, INPUT_DEVICE::KEYBOARD, INPUT_DEVICE::MAX);
}

//**********************************************************************************
// *** 入力受付の情報 ***
//**********************************************************************************
struct INPUT_CONTROL_PATH
{
	INPUT_DEVICE device = INPUT_DEVICE::NONE;		// デバイスの種類
	int nControl = 0;		// 入力を受け付けるキーの種類 (DIK / MOUSEKEY / JOYKEY / JOYSTICK)
	int nIdxDevice = 0;		// デバイス番号 (Joypadのみ使用)
};

//**********************************************************************************
// *** 入力受付の情報作成関数 ***
//**********************************************************************************
inline INPUT_CONTROL_PATH MakePathKeyboard(int nKey)
{
	return INPUT_CONTROL_PATH{ INPUT_DEVICE::KEYBOARD, nKey, 0 };
}

inline INPUT_CONTROL_PATH MakePathJoypadAxis(int nAxis)
{
	return INPUT_CONTROL_PATH{ INPUT_DEVICE::JOYPAD_AXIS, nAxis, 0 };
}
#endif