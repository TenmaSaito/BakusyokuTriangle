//==================================================================================
//
// ファイル入出力クラス！ [fliestream.h]
// Author : TENMA SAITO
// DATE : 2026/04/28
//
//==================================================================================
#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <format>
#include <variant>
#include <optional>
#include <span>

//**********************************************************************************
// *** ファイル入出力クラス ***
//**********************************************************************************
class CFile
{
public:
	// ファイルオープンモード
	enum class MODE
	{
		READ = 0,		// 読み込み
		WRITE,			// 書き込み (上書き)
		WRITE_IF,		// 書き込み (作成済みなら取り消し)
		APPEND,			// 書き込み (追記)
		MAX
	};

	// ファイルの状態
	enum class STATE
	{
		INVALID = 0,	// 無効 (ファイルが開いていない)
		READ,			// 読み込みモード
		WRITE,			// 書き込みモード
		MAX
	};

	CFile() noexcept;
	CFile(const CFile &file) = delete;
	CFile(CFile &&other) noexcept = default;
	~CFile();

	// File Relation Function
	bool Open(std::string_view sFilename, const MODE mode, const bool bBinary = false);
	void Close(void);
	bool IsOpen(void) const;
	bool IsState(const STATE state) const;
	uintmax_t GetSize(void) const { return m_size; }
	std::string_view GetPath(void) const { return m_filename; }
	std::variant<std::monostate, std::ifstream, std::ofstream> &GetStream(void) { return m_fstream; }

	// Text Reader Function
	std::optional<std::string> ReadLine(void);
	std::optional<std::string> ReadWord(void);
	std::optional<std::string> ReadUntil(const char cDelim);

	// Text Writer Function
	template<class T> bool WriteWord(const T &out);
	bool WriteString(std::string_view out);

	// Binary Reader Function
	bool ReadBytes(std::span<std::byte> in);
	template<class T> bool ReadType(T &in);
	template<class T> bool ReadType(std::span<T> in);

	// Binary Writer Function
	bool WriteBytes(std::span<const std::byte> out);
	template<class T> bool WriteType(const T &out);
	template<class T> bool WriteType(std::span<const T> out);

	static bool FindString(const std::string_view source, 
		const std::string_view s,
		size_t *pOut = nullptr,
		const bool bSkip = false);
	static int ToInt(const char *pStr, char **ppEnd = nullptr) { return std::strtol(pStr, ppEnd, 0); }
	static float ToFloat(const char *pStr, char **ppEnd = nullptr) { return std::strtof(pStr, ppEnd); }
	static Vector2 ToVector2(const char *pStr, char **ppEnd = nullptr, const bool bInt = false);
	static Vector3 ToVector3(const char *pStr, char **ppEnd = nullptr, const bool bInt = false);
	bool IsEoF(void) const;

	template<class... Args> static std::string ToString(std::_Fmt_string<Args...> fmt, Args... args);

	CFile &operator=(const CFile &file) = delete;
	CFile &operator=(CFile &&other) noexcept = default;

private:
	template<class FStream> FStream &Get(void) { return std::get<FStream>(m_fstream); }
	template<class FStream> const FStream &Get(void) const { return std::get<FStream>(m_fstream); }

	std::variant<std::monostate, std::ifstream, std::ofstream> m_fstream;		// 現在のファイルストリーム
	std::string m_filename;		// 開いているファイル名
	uintmax_t m_size = 0U;		// ファイルのサイズ
	bool m_bBinary = false;		// バイナリかどうか
};

//==================================================================================
// --- テキストファイルへの値の書き出し処理 ---
//==================================================================================
template<class T> bool CFile::WriteWord(const T &out)
{ // 出力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::WRITE) == false || m_bBinary == true) return false;

	// ストリーム演算子で書き出し
	Get<std::ofstream>() << out;
	return static_cast<bool>(Get<std::ofstream>());
}

//==================================================================================
// --- バイナリファイルからの値の読み込み処理 ---
//==================================================================================
template<class T> bool CFile::ReadType(T &in)
{ // 入力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::READ) == false || m_bBinary == false) return false;

	// バイトデータ配列にキャストして読み込み
	static_assert(std::is_trivially_copyable_v<T>);
	return ReadBytes(std::as_writable_bytes(std::span(&in, 1)));
}

//==================================================================================
// --- バイナリファイルからの値の読み込み処理 (配列) ---
//==================================================================================
template<class T> bool CFile::ReadType(std::span<T> in)
{ // 入力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::READ) == false || m_bBinary == false) return false;

	// バイトデータ配列にキャストして読み込み
	static_assert(std::is_trivially_copyable_v<T>);
	return ReadBytes(std::as_writable_bytes(in));
}

//==================================================================================
// --- バイナリファイルへの値の書き出し処理 ---
//==================================================================================
template<class T> bool CFile::WriteType(const T &in)
{ // 出力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::WRITE) == false || m_bBinary == false) return false;

	// バイトデータ配列にキャストして書き出し
	static_assert(std::is_trivially_copyable_v<T>);
	return WriteBytes(std::as_bytes(std::span(&in, 1)));
}

//==================================================================================
// --- バイナリファイルへの値の書き出し処理 (配列) ---
//==================================================================================
template<class T> bool CFile::WriteType(const std::span<const T> in)
{ // 出力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::WRITE) == false || m_bBinary == false) return false;

	// バイトデータ配列にキャストして書き出し
	static_assert(std::is_trivially_copyable_v<T>);
	return WriteBytes(std::as_bytes(in));
}

//==================================================================================
// --- 文字列化のヘルパー関数 ---
//==================================================================================
template<class... Args> static std::string CFile::ToString(std::_Fmt_string<Args...> fmt, Args... args)
{
	return std::format(fmt, std::forward<Args>(args)...);
}
#endif // !_FILESTREAM_H_