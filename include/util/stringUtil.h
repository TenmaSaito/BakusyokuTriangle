//==================================================================================
//
// 文字列の補助関数ヘッダーファイル [stringUtil.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _STRING_UTIL_H_		// インクルードガード
#define _STRING_UTIL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <string_view>
#include <optional>

//**********************************************************************************
// *** 文字列の補助関数名前空間 ***
//**********************************************************************************
namespace StringUtil
{
	// 文字列検索
	bool Contains(std::string_view source, std::string_view find);
	std::optional<std::string_view> GetValue(std::string_view line, std::string_view key);

	// 数値変換 (変換できなかった場合は0を返す)
	int ToInt(std::string_view str);
	float ToFloat(std::string_view str);
	Vector2 ToVector2(std::string_view str);
	Vector3 ToVector3(std::string_view str);
}
#endif // !_STRING_UTIL_H_
