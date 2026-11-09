//==================================================================================
// 
// インプットアクションクラスのソースファイル [inputAction.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input/inputAction.h"
#include "input/inputBinding.h"
#include "input/inputSystem.h"
#include "math/vec3math.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInputAction::CInputAction()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInputAction::~CInputAction()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInputAction::Init(std::string_view sName,
	const ACTION_TYPE type,
	const float fPressPoint)
{ // メンバ変数の初期化
	m_sName = sName;
	m_type = type;
	m_phase = PHASE::WAITING;
	m_value = VECTOR3_NULL;
	m_bPressedPrev = false;
	m_bPressedCurrent = false;
	m_fPressPoint = fPressPoint;
	m_callback = {};
	m_vpBinding.clear();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInputAction::Uninit(void)
{
	for (auto &pBinding : m_vpBinding)
	{ // バッファ破棄
		SafeUniqueUninit(pBinding);
	}

	// 配列をクリア
	m_vpBinding.clear();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CInputAction::Update(const CInputSystem &inputSystem)
{
	// 現状最も大きい値を持つVector3を計算し取得
	m_value = EvaluateBindings(inputSystem);		

	// 入力の大きさを取得
	const float fLength = Vec3::Length(m_value);

	// しきい値で押下状態を判定し、前フレームの結果を保存
	m_bPressedPrev = m_bPressedCurrent;
	m_bPressedCurrent = (fLength >= m_fPressPoint);

	// 今後コールバック関数を登録する際は、ここにm_phaseの更新処理を記述
}

//==================================================================================
// --- フラグリセット処理 ---
//==================================================================================
void CInputAction::Reset(void)
{
	m_phase = PHASE::WAITING;
	m_value = VECTOR3_NULL;
	m_bPressedPrev = false;
	m_bPressedCurrent = false;
}

//==================================================================================
// --- 入力の紐づけ登録処理 ---
//==================================================================================
void CInputAction::AddBinding(CInputBinding *pBinding)
{ // nullの場合アサート
	assert(pBinding != nullptr);

	// 配列に追加
	m_vpBinding.push_back(std::unique_ptr<CInputBinding>(pBinding));
}

//==================================================================================
// --- 全登録済み入力から最も大きな値を取得する処理 ---
//==================================================================================
Vector3 CInputAction::EvaluateBindings(const CInputSystem &inputSystem) const
{ // アクションのタイプが、無効ならアサート
	assert(IsValid(m_type));

	Vector3 value = VECTOR3_NULL;		// 取得した値
	Vector3 valueMax = VECTOR3_NULL;	// 最大値

	switch (m_type)
	{
	case CInputAction::ACTION_TYPE::BUTTON:		// 押下
	case CInputAction::ACTION_TYPE::VALUE_1D:	// 負方向 + 正方向
		for (auto &pBinding : m_vpBinding)
		{ // nullの場合スキップ
			if (pBinding == nullptr) continue;

			// 値を取得
			value.x = pBinding->ReadValue1D(inputSystem);
			if (fabsf(value.x) > fabsf(valueMax.x))
			{ // 最大値よりも大きかった場合、最大値を更新
				valueMax.x = value.x;
			}
		}
		break;

	case CInputAction::ACTION_TYPE::VALUE_2D:	// 2D値
		for (auto &pBinding : m_vpBinding)
		{ // nullの場合スキップ
			if (pBinding == nullptr) continue;

			// 値を取得
			value = pBinding->ReadValue2D(inputSystem);
			if (Vec3::Length(value) > Vec3::Length(valueMax))
			{ // 最大値よりも大きかった場合、最大値を更新
				valueMax = value;
			}
		}
		break;

	default:
		break;
	}

	return valueMax;
}