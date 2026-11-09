//==================================================================================
// 
// インプットアクションのマップクラスのソースファイル [inputActionMap.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input/inputActionMap.h"
#include <algorithm>

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInputActionMap::CInputActionMap()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInputActionMap::~CInputActionMap()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInputActionMap::Init(std::string_view sName)
{ // メンバ変数の初期化
	m_sName = sName;	// 名前を保存
	m_bEnable = true;	// 有効化

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInputActionMap::Uninit(void)
{
	for (auto &pAction : m_vpAction)
	{ // バッファ破棄
		SafeUniqueUninit(pAction);
	}

	// 配列クリア
	m_vpAction.clear();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CInputActionMap::Update(const CInputSystem &inputSystem)
{ // 無効ならスキップ
	if (m_bEnable != true) return;

	for (auto &pAction : m_vpAction)
	{  // nullの場合スキップ
		if (pAction == nullptr) continue;

		// 更新
		pAction->Update(inputSystem);
	}
}

//==================================================================================
// --- インプットアクションの追加処理 ---
//==================================================================================
CInputAction *CInputActionMap::Add(std::string_view sName,
	const CInputAction::ACTION_TYPE type,
	const float fPressPoint)
{ // 名前が空もしくは、タイプが無効な場合アサート + スキップ
	assert(!sName.empty()
		&& CInputAction::IsValid(type));
	if (sName.empty() 
		|| !CInputAction::IsValid(type)) return nullptr;

	// インプットアクションの生成
	auto &pAction = m_vpAction.emplace_back(std::make_unique<CInputAction>());
	pAction->Init(sName, type, fPressPoint);

	// 生成したインプットアクションのポインタを返す
	return pAction.get();
}

//==================================================================================
// --- インプットアクションの検索処理 ---
//==================================================================================
CInputAction *CInputActionMap::Find(std::string_view sName) const
{ // 名前がnullの場合アサート + スキップ
	assert(!sName.empty());
	if (sName.empty()) return nullptr;

	for (auto &pAction : m_vpAction)
	{  // nullの場合スキップ
		if (pAction == nullptr) continue;

		if (pAction->IsEqualName(sName) == true)
		{ // もし同じ名前ならば、ポインタを返す
			return pAction.get();
		}
	}

	return nullptr;
}

//==================================================================================
// --- アクションマップの更新有効化設定処理 ---
//==================================================================================
void CInputActionMap::SetEnable(const bool bEnable)
{
	bool bEnablePrev = m_bEnable;	// 直前のフラグ
	m_bEnable = bEnable;			// フラグを適用

	// 既に設定済み、もしくは有効にした場合はスキップ
	if (bEnablePrev == bEnable || bEnable == true) return;

	for (auto &pAction : m_vpAction)
	{  // nullの場合スキップ
		if (pAction == nullptr) continue;

		// 更新されなくなるため、フラグをリセット
		pAction->Reset();
	}
}