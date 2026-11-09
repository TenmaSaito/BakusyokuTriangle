//==================================================================================
// 
// ジョイパッドクラスのヘッダーファイル [joypad.h]
// Author : TENMA SAITO
// Date   : 2026/5/15
// 
//==================================================================================
#ifndef _JOYPAD_H_
#define _JOYPAD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int MAX_JOYPAD = 2;			// ジョイパッドの最大数
constexpr int JOYTHUMB_REPEAT_NUM = 2;	// ジョイパッドでリピート対応しているスティックの数
constexpr int JOYPAD_DEFAULT_WAIT_PRESS = 2;		// リピートの連続入力までの待機時間
constexpr int JOYPAD_DEFAULT_INTERVAL = 2;			// リピートの連続入力の間隔
constexpr float STICK_DEADZONE = 0.05f;				// 動いたと感知するデッドゾーン
constexpr float JOYPAD_DEFAULT_STICK_RANGE = 0.05f;	// スティックを倒したと判定する範囲

//**********************************************************************************
// *** ジョイパッドクラス ***
//**********************************************************************************
class CJoypad
{
public:
	//******************************************************************************
	// *** ジョイパッドのボタン入力一覧 ***
	//******************************************************************************
	enum class KEY
	{
		UP = 0,				// [00] 十字キー(上)
		DOWN,				// [01] 十字キー(下)
		LEFT,				// [02] 十字キー(左)
		RIGHT,				// [03] 十字キー(右)
		START,				// [04] ボタンキー(START)
		BACK,				// [05] ボタンキー(BACK)
		LEFT_PUSH,			// [06] スティックキー(左押し込み)
		RIGHT_PUSH,			// [07] スティックキー(右押し込み)
		LB,					// [08] ボタンキー(LB)
		RB,					// [09] ボタンキー(RB)
		NONE1,				// [10] 予約済み
		NONE2,				// [11] 予約済み
		A,					// [12] ボタンキー(A)
		B,					// [13] ボタンキー(B)
		X,					// [14] ボタンキー(X)
		Y,					// [15] ボタンキー(Y)
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const KEY value) noexcept
	{
		return EnumUtil::InRange(value, KEY::UP, KEY::MAX)
			&& value != KEY::NONE1 && value != KEY::NONE2;	// 予約済みは無効
	}

	//******************************************************************************
	// *** ジョイスティック入力一覧 ***
	//******************************************************************************
	enum class STICK
	{
		LEFT,				// [01] 左スティック(スティック取得) 
		LEFT_UP,			// [02] 左スティック(上)
		LEFT_DOWN,		// [03] 左スティック(下)
		LEFT_LEFT,		// [04] 左スティック(左)
		LEFT_RIGHT,		// [05] 左スティック(右)
		RIGHT,			// [06] 左スティック(スティック取得)
		RIGHT_UP,			// [07] 右スティック(上)
		RIGHT_DOWN,		// [08] 右スティック(下)
		RIGHT_LEFT,		// [09] 右スティック(左)
		RIGHT_RIGHT,		// [10] 右スティック(右)
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const STICK value) noexcept
	{
		return EnumUtil::InRange(value, STICK::LEFT, STICK::MAX);
	}

	//******************************************************************************
	// *** ジョイスティック入力方向一覧 ***
	//******************************************************************************
	enum class AXIS
	{
		LEFT_X = 0,		// 左スティックのX軸
		LEFT_Y,			// 左スティックのY軸
		RIGHT_X,			// 右スティックのX軸
		RIGHT_Y,			// 右スティックのY軸
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const AXIS value) noexcept
	{
		return EnumUtil::InRange(value, AXIS::LEFT_X, AXIS::MAX);
	}

	CJoypad();
	~CJoypad();

	void Init(void);
	void Uninit(void);
	void Update(void);
	
	bool GetPress(const KEY key, const int nPadID = 0);
	bool GetTrigger(const KEY key, const int nPadID = 0);
	bool GetRelease(const KEY key, const int nPadID = 0);
	bool GetRepeat(const KEY key,
		const int nWaitPress = JOYPAD_DEFAULT_WAIT_PRESS,
		const int nInterval = JOYPAD_DEFAULT_INTERVAL,
		const int nPadID = 0);
	float GetAxis(const AXIS axis, const int nPadID = 0);

	bool GetStick(const STICK stick,
		const float fRange = JOYPAD_DEFAULT_STICK_RANGE,
		const int nPadID = 0);

	bool GetStick(const STICK stick, 
		Vector3 *pOut, 
		const int nPadID = 0);

	bool GetStickRepeat(const STICK stick, 
		const float fRange = JOYPAD_DEFAULT_STICK_RANGE,
		const int nWaitPress = JOYPAD_DEFAULT_WAIT_PRESS,
		const int nInterval = JOYPAD_DEFAULT_INTERVAL,
		const int nPadID = 0);

	void SetVibration(const int nLPower,
		const int nRPower,
		const int nTime,
		const int nPadID = 0);

private:
	//******************************************************************************
	// *** ジョイパッドの情報構造体 ***
	//******************************************************************************
	typedef struct
	{
		XINPUT_STATE joykeyState;					// ジョイパッドのプレス情報
		XINPUT_STATE joykeyStateTrigger;			// ジョイパッドのトリガー情報
		XINPUT_STATE joykeyStateRelease;			// ジョイパッドのリリース情報
		int	nJoykeyStateRepeat[EnumUtil::ToIndex(KEY::MAX)];			// ジョイパッドのリピート情報
		int nJoythumbRepeat[EnumUtil::ToIndex(STICK::MAX)];				// ジョイスティックのリピート情報
		XINPUT_VIBRATION vibration;					// バイブレーションの情報
		int nCounterVibration;						// バイブレーションの時間
	} XINPUT_INFO;

	XINPUT_INFO m_aJoypad[MAX_JOYPAD] = {};			// ジョイパッドの情報
};
#endif