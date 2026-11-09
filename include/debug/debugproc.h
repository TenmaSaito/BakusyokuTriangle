//==================================================================================
// 
// デバッグプロシージャクラスのヘッダーファイル [debugproc.h]
// Author : TENMA SAITO
// Date   : 2026/5/17
// 
//==================================================================================
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include <format>
#include <type_traits>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define PRINT_VECTOR2(vec)	(vec).x, (vec).y			// VECTOR2の表示簡略マクロ
#define PRINT_VECTOR3(vec)	(vec).x, (vec).y, (vec).z		// VECTOR3の表示簡略マクロ

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int DEFAULT_SIZE = 23;	// 基本サイズ
constexpr int DEFAULT_STRING_CAPACITY = 2048;	// 文字数の初期サイズ
constexpr std::string_view DEFAULT_FONT = "Terminal";	// 基本フォント

//**********************************************************************************
// *** デバッグプロシージャクラス ***
//**********************************************************************************
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	HRESULT Init(const UINT &rHeight, const char *pFontname);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	template<class... Args> void Print(std::_Fmt_string<Args...> format, Args ...args);
	template<class... Args> void ForcePrint(std::_Fmt_string<Args...> format, Args ...args);

private:
	LPD3DXFONT m_pFont;		// フォントへのポインタ
	Color m_colFont;		// フォントカラー
	std::string m_sProc;	// 表示する文字列
};

//==================================================================================
// --- デバッグ表示の追加処理 (デバッグ時無効) ---
//==================================================================================
template<class... Args> void CDebugProc::Print(std::_Fmt_string<Args...> format, Args ...args)
{
#ifdef _DEBUG
	m_sProc += std::format(format, std::forward<Args>(args)...);
#endif
}

//==================================================================================
// --- デバッグ表示の追加処理 (強制表示) ---
//==================================================================================
template<class... Args> void CDebugProc::ForcePrint(std::_Fmt_string<Args...> format, Args ...args)
{
#if 0
	m_sProc += std::format(format, std::forward<Args>(args)...);
#endif
}
#endif