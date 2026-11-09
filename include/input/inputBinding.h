//==================================================================================
// 
// 物理入力の紐づけクラスのヘッダーファイル [inputBinding.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _INPUT_BINDING_H_		// インクルードガード
#define _INPUT_BINDING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"
#include "input/inputControl.h"
#include <array>
#include <vector>
#include <memory>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class IInputProcessor;
class CInputSystem;

//**********************************************************************************
// *** 物理入力の紐づけクラス ***
//**********************************************************************************
class CInputBinding
{
public:
    enum class TYPE
    {
		NONE = 0,	// 無し
        SINGLE,		// 1つのコントロール(Spaceなど) / 使用配列 : [0]
        AXIS_1D,    // 負方向 + 正方向 / 使用配列 : [0]=負 / [1]=正 
        VECTOR_2D,  // 上下左右の4コントロールを合成(WASD) / 使用配列 : [0]=上, [1]=下, [2]=左, [3]=右
		STICK_1D,	// スティック1本を軸指定して読む / 使用配列 : [0]
        STICK_2D,   // スティック1本をそのまま2D値として読む / 使用配列 : [0]
		MAX
    };

    // 有効値かどうかの判定
    static constexpr bool IsValid(const TYPE value) noexcept
    {
    	return EnumUtil::InRange(value, TYPE::SINGLE, TYPE::MAX);
    }

    CInputBinding();
    ~CInputBinding();

	static CInputBinding *CreateSingle(const INPUT_CONTROL_PATH &path);
	static CInputBinding *CreateAxis1D(const INPUT_CONTROL_PATH &pathNegative,
		const INPUT_CONTROL_PATH &pathPositive);
	static CInputBinding *CreateVector2D(const INPUT_CONTROL_PATH &pathUp,
		const INPUT_CONTROL_PATH &pathDown,
		const INPUT_CONTROL_PATH &pathLeft,
		const INPUT_CONTROL_PATH &pathRight);
	static CInputBinding *CreateStick1D(const INPUT_CONTROL_PATH &pathStick);
	static CInputBinding *CreateStick2D(const INPUT_CONTROL_PATH &pathStick);

	HRESULT Init(void);
	void Uninit(void);
	void SetSingle(const INPUT_CONTROL_PATH &path);
	void SetAxis1D(const INPUT_CONTROL_PATH &pathNegative,
		const INPUT_CONTROL_PATH &pathPositive);
	void SetVector2D(const INPUT_CONTROL_PATH &pathUp,
		const INPUT_CONTROL_PATH &pathDown,
		const INPUT_CONTROL_PATH &pathLeft,
		const INPUT_CONTROL_PATH &pathRight);
	void SetStick1D(const INPUT_CONTROL_PATH &pathStick);
	void SetStick2D(const INPUT_CONTROL_PATH &pathStick);

    float ReadValue1D(const CInputSystem &inputSystem) const;
    Vector3 ReadValue2D(const CInputSystem &inputSystem) const;

private:
	static constexpr int PATH_MAX_COUNT = 4;	// 紐ずけられる入力の最大数
	TYPE m_type = TYPE::NONE;					// 読み取り(合成)方法の種類
	std::array<INPUT_CONTROL_PATH, PATH_MAX_COUNT> m_aPath = {};	// SINGLE:[0] / 1D:負,正 / 2D:上,下,左,右
    std::vector<std::unique_ptr<IInputProcessor>> m_vpProcessor;	// 読み取った値に順に適用する加工処理
};
#endif