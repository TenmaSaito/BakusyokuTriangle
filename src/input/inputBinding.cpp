//==================================================================================
// 
// 物理入力の紐づけクラスのソースファイル [inputBinding.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input/inputBinding.h"
#include "input/inputSystem.h"
#include "input/inputProcessor.h"
#include "math/vec3math.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	// VECTOR_2D で使用する配列の添え字
	constexpr int PATH_UP = 0;		// 上
	constexpr int PATH_DOWN = 1;	// 下
	constexpr int PATH_LEFT = 2;	// 左
	constexpr int PATH_RIGHT = 3;	// 右
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInputBinding::CInputBinding()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInputBinding::~CInputBinding()
{
}

//==================================================================================
// --- 単発入力生成処理 ---
//==================================================================================
CInputBinding *CInputBinding::CreateSingle(const INPUT_CONTROL_PATH &path)
{
	CInputBinding *pBindingKey = new CInputBinding();		// 生成した入力へのポインタ
	if (pBindingKey != nullptr)
	{ // 生成出来ていれば初期化 + 入力登録
		pBindingKey->Init();
		pBindingKey->SetSingle(path);
	}

	return pBindingKey;
}

//==================================================================================
// --- 正負の軸値入力生成処理 ---
//==================================================================================
CInputBinding *CInputBinding::CreateAxis1D(const INPUT_CONTROL_PATH &pathNegative,
	const INPUT_CONTROL_PATH &pathPositive)
{
	CInputBinding *pBindingKeys = new CInputBinding();		// 生成した入力へのポインタ
	if (pBindingKeys != nullptr)
	{ // 生成出来ていれば初期化 + 入力登録
		pBindingKeys->Init();
		pBindingKeys->SetAxis1D(pathNegative, pathPositive);
	}

	return pBindingKeys;
}

//==================================================================================
// --- 上下左右のコントロール入力生成処理 ---
//==================================================================================
CInputBinding *CInputBinding::CreateVector2D(const INPUT_CONTROL_PATH &pathUp,
	const INPUT_CONTROL_PATH &pathDown,
	const INPUT_CONTROL_PATH &pathLeft,
	const INPUT_CONTROL_PATH &pathRight)
{
	CInputBinding *pBindingKeys = new CInputBinding();		// 生成した入力へのポインタ
	if (pBindingKeys != nullptr)
	{ // 生成出来ていれば初期化 + 入力登録
		pBindingKeys->Init();
		pBindingKeys->SetVector2D(pathUp, pathDown, pathLeft, pathRight);
	}

	return pBindingKeys;
}

//==================================================================================
// --- 軸指定スティック入力生成処理 ---
//==================================================================================
CInputBinding *CInputBinding::CreateStick1D(const INPUT_CONTROL_PATH &pathStick)
{
	CInputBinding *pBindingKey = new CInputBinding();		// 生成した入力へのポインタ
	if (pBindingKey != nullptr)
	{ // 生成出来ていれば初期化 + 入力登録
		pBindingKey->Init();
		pBindingKey->SetStick1D(pathStick);
	}

	return pBindingKey;
}

//==================================================================================
// --- スティック入力生成処理 ---
//==================================================================================
CInputBinding *CInputBinding::CreateStick2D(const INPUT_CONTROL_PATH &pathStick)
{
	CInputBinding *pBindingKey = new CInputBinding();		// 生成した入力へのポインタ
	if (pBindingKey != nullptr)
	{ // 生成出来ていれば初期化 + 入力登録
		pBindingKey->Init();
		pBindingKey->SetStick2D(pathStick);
	}

	return pBindingKey;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInputBinding::Init(void)
{ // メンバ変数の初期化
	m_type = TYPE::NONE;
	m_aPath = {};
	m_vpProcessor.clear();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInputBinding::Uninit(void)
{ 
	for (auto &pProcessor : m_vpProcessor)
	{ // nullの場合スキップ
		if (pProcessor == nullptr) continue;
		
		// バッファ破棄
		pProcessor.reset();
	}

	// 配列をクリア
	m_vpProcessor.clear();
}

//==================================================================================
// --- 単発入力登録処理 ---
//==================================================================================
void CInputBinding::SetSingle(const INPUT_CONTROL_PATH &path)
{
	m_type = TYPE::SINGLE;		// 単発入力を設定
	m_aPath[0] = path;			// 引数を保存
}

//==================================================================================
// --- 正負の軸値入力登録処理 ---
//==================================================================================
void CInputBinding::SetAxis1D(const INPUT_CONTROL_PATH &pathNegative,
	const INPUT_CONTROL_PATH &pathPositive)
{
	m_type = TYPE::AXIS_1D;		// 軸値入力を設定
	m_aPath[0] = pathNegative;	// 負の引数を保存
	m_aPath[1] = pathPositive;	// 正の引数を保存
}

//==================================================================================
// --- 上下左右のコントロール入力登録処理 ---
//==================================================================================
void CInputBinding::SetVector2D(const INPUT_CONTROL_PATH &pathUp,
	const INPUT_CONTROL_PATH &pathDown,
	const INPUT_CONTROL_PATH &pathLeft,
	const INPUT_CONTROL_PATH &pathRight)
{
	m_type = TYPE::VECTOR_2D;	// 上下左右のコントロール入力を設定
	m_aPath[PATH_UP] = pathUp;		// 上方向の引数を保存
	m_aPath[PATH_DOWN] = pathDown;		// 下方向の引数を保存
	m_aPath[PATH_LEFT] = pathLeft;		// 左方向の引数を保存
	m_aPath[PATH_RIGHT] = pathRight;		// 右方向の引数を保存
}

//==================================================================================
// --- 軸指定スティック入力登録処理 ---
//==================================================================================
void CInputBinding::SetStick1D(const INPUT_CONTROL_PATH &path)
{
	m_type = TYPE::STICK_1D;	// 軸指定スティック入力を設定
	m_aPath[0] = path;			// 引数を保存
}

//==================================================================================
// --- スティック入力登録処理 ---
//==================================================================================
void CInputBinding::SetStick2D(const INPUT_CONTROL_PATH &path)
{
	m_type = TYPE::STICK_2D;	// スティック入力を設定
	m_aPath[0] = path;			// 引数を保存
}

//==================================================================================
// --- 単発・軸値取得処理 ---
//==================================================================================
float CInputBinding::ReadValue1D(const CInputSystem &inputSystem) const
{ // 単発・軸値・軸指定スティック以外が登録されている場合、アサート
	assert(m_type == TYPE::SINGLE || m_type == TYPE::AXIS_1D || m_type == TYPE::STICK_1D);

	float fValue = 0.0f;		// 値
	float fPositive = 0.0f;		// 正の値
	float fNegative = 0.0f;		// 負の値

	switch (m_type)
	{
	case TYPE::SINGLE:			// 単発入力
	case TYPE::STICK_1D:		// 軸指定スティック
		fValue = inputSystem.ReadControl(m_aPath[0]);
		break;

	case TYPE::AXIS_1D:			// 軸値入力
		fNegative = inputSystem.ReadControl(m_aPath[0]);
		fPositive = inputSystem.ReadControl(m_aPath[1]);
		fValue = fPositive - fNegative;
		break;

	default:
		break;
	}

	return fValue;
}

//==================================================================================
// --- 上下左右・スティック取得処理 ---
//==================================================================================
Vector3 CInputBinding::ReadValue2D(const CInputSystem &inputSystem) const
{ // 上下左右・スティック以外が登録されている場合、アサート
	assert(m_type == TYPE::VECTOR_2D || m_type == TYPE::STICK_2D);

	Vector3 value = VECTOR3_NULL;		// 値
	float fUpper = 0.0f;	// 上方向の値
	float fLower = 0.0f;	// 下方向の値
	float fLeft = 0.0f;		// 左方向の値
	float fRight = 0.0f;	// 右方向の値

	switch (m_type)
	{
	case CInputBinding::TYPE::VECTOR_2D:		// 上下左右入力
		fUpper = inputSystem.ReadControl(m_aPath[PATH_UP]);
		fLower = inputSystem.ReadControl(m_aPath[PATH_DOWN]);
		fLeft = inputSystem.ReadControl(m_aPath[PATH_LEFT]);
		fRight = inputSystem.ReadControl(m_aPath[PATH_RIGHT]);
		value = Vector3(fRight - fLeft, fUpper - fLower, 0.0f);

		if (Vec3::Length(value) > 1.0f)
		{ // 斜め入力で速くならないよう、長さを1に収める
			value = Vec3::Normalize(value);
		}
		break;

	case CInputBinding::TYPE::STICK_2D:			// スティック入力
		value = inputSystem.ReadStick(m_aPath[0]);
		break;

	default:
		break;
	}

	return value;
}