//==================================================================================
// 
// アイテムスクリプト読み込みクラスのソースファイル [itemLoader.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/item/itemLoader.h"
#include "game/item/itemSpawner.h"
#include "game/item/eatingRule.h"
#include "util/stringUtil.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	// スクリプトの読み込む各キーワードの意味
	constexpr std::string_view SPAWNER_START = "SETSPAWNER";	// スポナー用データの開始位置
	constexpr std::string_view SPAWNER_END = "END_SETSPAWNER";	// スポナー用データの終了位置
	constexpr std::string_view SET_SPAWN_FIELD_MARGIN = "SPAWN_FIELD_MARGIN = ";				// ステージの端から離す距離
	constexpr std::string_view SET_DISTANCE_FROM_PLAYER = "DISTANCE_FROM_PLAYER = ";			// アイテムとプレイヤーの最短距離
	constexpr std::string_view SET_DISTANCE_FROM_OTHER_ITEM = "DISTANCE_FROM_OTHER_ITEM = ";	// アイテム動詞の最短距離
	constexpr std::string_view SET_ITEM_RADIUS = "ITEM_RADIUS = ";				// アイテムの当たり判定の半径
	constexpr std::string_view SET_TRY_IF_FAILED = "TRY_IF_FAILED = ";			// 距離制限による設置失敗時に行う試行回数の最大

	constexpr std::string_view SCORE_START = "SETSCORE";	// スコア用データの開始位置
	constexpr std::string_view SCORE_END = "END_SETSCORE";	// スコア用データの終了位置
	constexpr std::string_view RICE = "RICE = ";				// 米のスコア
	constexpr std::string_view SIDE_DISH = "SIDE_DISH = ";		// おかずのスコア
	constexpr std::string_view SOUP = "SOUP = ";				// みそ汁のスコア
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CItemLoader::CItemLoader()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CItemLoader::~CItemLoader()
{
}

//==================================================================================
// --- スクリプト読み込み処理 ---
//==================================================================================
HRESULT CItemLoader::Load(std::string_view path)
{ // null参照防止
	if (m_pSpawner == nullptr || m_pRule == nullptr) return E_POINTER;

	if (Open(path) == false)
	{ // ファイルオープン失敗時
		return E_FAIL;
	}

	if (SeekScriptBegin() == false)
	{ // スクリプト開始キーワードが見つからなかった場合
		return E_FAIL;
	}

	HRESULT hr = S_OK;		// 読み込み結果
	std::optional<std::string> line;		// 読み込んだ文字列

	while (1)
	{ // 文字列を一列読み込み
		line = ReadCleanLine();
		if (line.has_value() == false || IsScriptEnd(*line))
		{ // 読み込み終了
			break;
		}
		
		std::string &sLine = line.value();		// 読み込んだ文字列
		if (StringUtil::Contains(sLine, SPAWNER_START))
		{ // スポナー情報
			LoadSpawner();
		}
		else if (StringUtil::Contains(sLine, SCORE_START))
		{ // スコア情報
			LoadEatingRule();
		}
	}

	// ファイルを閉じる
	Close();

	return S_OK;		// 読み込み成功
}

//==================================================================================
// --- スポナー関連のデータ読み込み処理 ---
//==================================================================================
void CItemLoader::LoadSpawner(void)
{
	std::optional<std::string> line;		// 読み込んだ文字列

	while (1)
	{ // 文字列を一列読み込み
		line = ReadCleanLine();
		if (line.has_value() == false || IsScriptEnd(*line))
		{ // 読み込み終了
			break;
		}

		std::string &sLine = line.value();		// 読み込んだ文字列
		if (StringUtil::Contains(sLine, SPAWNER_END))
		{ // スポナー関連のデータが終了した場合、ループ終了
			break;
		}
		else if (auto value = StringUtil::GetValue(sLine, SET_SPAWN_FIELD_MARGIN))
		{ // ステージの端から離す距離を設定
			m_pSpawner->SetSpawnFieldMargin(StringUtil::ToFloat(*value));
		}
		else if (auto value = StringUtil::GetValue(sLine, SET_DISTANCE_FROM_PLAYER))
		{ // プレイヤーから離す距離を設定
			m_pSpawner->SetDistanceFromPlayer(StringUtil::ToFloat(*value));
		}
		else if (auto value = StringUtil::GetValue(sLine, SET_DISTANCE_FROM_OTHER_ITEM))
		{ // 他のアイテムから離す距離を設定
			m_pSpawner->SetDistanceFromOtherItem(StringUtil::ToFloat(*value));
		}
		else if (auto value = StringUtil::GetValue(sLine, SET_ITEM_RADIUS))
		{ // アイテムの当たり判定半径を設定
			m_pSpawner->SetItemRadius(StringUtil::ToFloat(*value));
		}
		else if (auto value = StringUtil::GetValue(sLine, SET_TRY_IF_FAILED))
		{ // 設置の試行回数を設定
			m_pSpawner->SetTryIfFailed(StringUtil::ToInt(*value));
		}
	}
}

//==================================================================================
// --- スコア関連のデータ読み込み処理 ---
//==================================================================================
void CItemLoader::LoadEatingRule(void)
{
	std::optional<std::string> line;		// 読み込んだ文字列

	while (1)
	{ // 文字列を一列読み込み
		line = ReadCleanLine();
		if (line.has_value() == false || IsScriptEnd(*line))
		{ // 読み込み終了
			break;
		}

		std::string &sLine = line.value();		// 読み込んだ文字列
		if (StringUtil::Contains(sLine, SCORE_END))
		{ // スコア関連のデータが終了した場合、ループ終了
			break;
		}
		else if (auto value = StringUtil::GetValue(sLine, RICE))
		{ // 米のスコアを設定
			m_pRule->SetItemScore(CItem::TYPE::RICE, StringUtil::ToInt(*value));
		}
		else if (auto value = StringUtil::GetValue(sLine, SIDE_DISH))
		{ // おかずのスコアを設定
			m_pRule->SetItemScore(CItem::TYPE::SIDE_DISH, StringUtil::ToInt(*value));
		}
		else if (auto value = StringUtil::GetValue(sLine, SOUP))
		{ // みそ汁のスコアを設定
			m_pRule->SetItemScore(CItem::TYPE::SOUP, StringUtil::ToInt(*value));
		}
	}
}