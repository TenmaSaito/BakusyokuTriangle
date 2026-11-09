//==================================================================================
// 
// インプットアクションクラスのヘッダーファイル [inputAction.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _INPUT_ACTION_H_		// インクルードガード
#define _INPUT_ACTION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <functional>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CInputBinding;
class CInputSystem;

//**********************************************************************************
// *** インプットアクションクラス ***
//**********************************************************************************
class CInputAction
{
public:
    // アクションの受け付ける入力の種類
    enum class ACTION_TYPE
    {
		NONE = 0,		// 無し
        BUTTON,		    // 押下
        VALUE_1D,       // 負方向 + 正方向
        VALUE_2D,       // 2D値(WASD合成 / スティック)
		MAX
    };

    // 有効値かどうかの判定
    static constexpr bool IsValid(const ACTION_TYPE value) noexcept
    {
    	return EnumUtil::InRange(value, ACTION_TYPE::BUTTON, ACTION_TYPE::MAX);
    }

    // アクションの進行状態
    enum class PHASE 
    { 
		NONE = 0,		// 無し
        WAITING,		// 待機中
        STARTED,        // 押し始め
        PERFORMED,      // アクションが成立
        CANCELED,       // 入力が終了
		MAX
    };

    // 有効値かどうかの判定
    static constexpr bool IsValid(const PHASE value) noexcept
    {
    	return EnumUtil::InRange(value, PHASE::WAITING, PHASE::MAX);
    }

	CInputAction();
	~CInputAction();

	HRESULT Init(std::string_view sName, 
		const ACTION_TYPE type, 
		const float fPressPoint = DEFAULT_PRESS_POINT);
	void Uninit(void);
    void Update(const CInputSystem &inputSystem);

	void Reset(void);
	bool IsPressed(void) const { return m_bPressedCurrent; }
	bool WasPressedThisFrame(void) const { return m_bPressedCurrent && !m_bPressedPrev; }
	bool WasReleasedThisFrame(void) const { return !m_bPressedCurrent && m_bPressedPrev; }
	float ReadValue1D(void) const { return m_value.x; }
	const Vector3 &ReadValue2D(void) const { return m_value; }
	bool IsEqualName(std::string_view sName) const { return sName == m_sName; }

	void AddBinding(CInputBinding *pBinding);
	void SetOnPerformed(std::function<void(const CInputAction &)> callback) { m_callback = std::move(callback); }
	static constexpr float GetDefaultPressPoint(void) { return DEFAULT_PRESS_POINT; }

private:
	Vector3 EvaluateBindings(const CInputSystem &inputSystem) const;

	static constexpr float DEFAULT_PRESS_POINT = 0.5f;		// デフォルトの押されたとみなすしきい値
    std::string m_sName;		// アクションの名前
    ACTION_TYPE m_type = ACTION_TYPE::NONE;	  // アクションの受け付ける入力種類
    PHASE m_phase = PHASE::NONE;              // アクションの進行状態
	Vector3 m_value = VECTOR3_NULL;		// 現在の値 (BUTTON/1Dはxのみ使用、2Dはx,yを使用)
	bool m_bPressedPrev = false;		// 前フレームで「押されている」状態だったか
	bool m_bPressedCurrent = false;		// 今フレームで「押されている」状態か
	float m_fPressPoint = 0.0f;			// 押されたとみなすしきい値(既定 0.5)
    std::vector<std::unique_ptr<CInputBinding>> m_vpBinding;	// このアクションに紐づく物理入力 (複数ある場合は大きい値を採用)
	std::function<void(const CInputAction &)> m_callback;		// コールバック関数
};
#endif