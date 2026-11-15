//==================================================================================
//
// 文字列の補助関数ソースファイル [stringUtil.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "util/stringUtil.h"
#include <charconv>

//**********************************************************************************
// *** 補助関数 ***
//**********************************************************************************
namespace
{
	//==================================================================================
	// --- 先頭の空白を読み飛ばす処理 ---
	//==================================================================================
	const char *SkipSpace(const char *pFirst, const char *pLast)
	{ // 空白・タブ・'+'記号を読み飛ばす (from_charsは先頭の空白と'+'を受け付けないため)
		while (pFirst != pLast && (*pFirst == ' ' || *pFirst == '\t' || *pFirst == '+'))
		{
			pFirst++;
		}
		return pFirst;
	}

	//==================================================================================
	// --- 数値を1つ読み込み、次の読み込み位置を返す処理 ---
	//==================================================================================
	template<class T> const char *Parse(const char *pFirst, const char *pLast, T &out)
	{ // 空白を読み飛ばしてから変換 (失敗時は0のまま)
		out = T();
		pFirst = SkipSpace(pFirst, pLast);
		auto [pNext, ec] = std::from_chars(pFirst, pLast, out);
		return (ec == std::errc()) ? pNext : pFirst;
	}
}

//==================================================================================
// --- 文字列が含まれているかの確認処理 ---
//==================================================================================
bool StringUtil::Contains(std::string_view source, std::string_view find)
{
	return source.find(find) != std::string_view::npos;
}

//==================================================================================
// --- キーの後ろに続く値の取得処理 ---
//
// 例 : GetValue("POS = 1 2 3", "POS = ") -> "1 2 3"
//==================================================================================
std::optional<std::string_view> StringUtil::GetValue(std::string_view line, std::string_view key)
{ // キーが見つからなければ無効値を返す
	size_t pos = line.find(key);
	if (pos == std::string_view::npos) return std::nullopt;

	// キーの後ろを返す
	return line.substr(pos + key.length());
}

//==================================================================================
// --- 文字列からのint変換処理 ---
//==================================================================================
int StringUtil::ToInt(std::string_view str)
{
	int nValue = 0;		// 結果
	Parse(str.data(), str.data() + str.size(), nValue);
	return nValue;
}

//==================================================================================
// --- 文字列からのfloat変換処理 ---
//==================================================================================
float StringUtil::ToFloat(std::string_view str)
{
	float fValue = 0.0f;	// 結果
	Parse(str.data(), str.data() + str.size(), fValue);
	return fValue;
}

//==================================================================================
// --- 文字列からのVector2変換処理 ---
//==================================================================================
Vector2 StringUtil::ToVector2(std::string_view str)
{
	const char *pCur = str.data();				// 現在の読み込み位置
	const char *pLast = str.data() + str.size();	// 終端
	Vector2 ret;		// 結果

	pCur = Parse(pCur, pLast, ret.x);
	pCur = Parse(pCur, pLast, ret.y);
	return ret;
}

//==================================================================================
// --- 文字列からのVector3変換処理 ---
//==================================================================================
Vector3 StringUtil::ToVector3(std::string_view str)
{
	const char *pCur = str.data();				// 現在の読み込み位置
	const char *pLast = str.data() + str.size();	// 終端
	Vector3 ret;		// 結果

	pCur = Parse(pCur, pLast, ret.x);
	pCur = Parse(pCur, pLast, ret.y);
	pCur = Parse(pCur, pLast, ret.z);
	return ret;
}
