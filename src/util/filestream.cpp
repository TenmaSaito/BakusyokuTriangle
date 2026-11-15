//==================================================================================
//
// ファイル入出力クラスの作成！ [fliestream.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "util/filestream.h"
#include <filesystem>

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CFileStream::CFileStream() noexcept
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CFileStream::~CFileStream()
{ // ファイルクローズ
	Close();
}

//==================================================================================
// --- ファイルオープン処理 ---
//==================================================================================
bool CFileStream::Open(std::string_view sPath, const MODE mode, const bool bBinary)
{ // ファイルを閉じる
	Close();

	bool bFound = false;		// ファイルが見つかったか

	// ファイルを検索
	bFound = std::filesystem::is_regular_file(sPath);

	if (mode == MODE::READ)
	{ // 読み込みモードの場合
		if (bFound == true)
		{ // オープン後サイズを調べ、開けたかを返す (書き込みのスコープを減らすためにreturn)
			m_fstream.emplace<std::ifstream>(sPath, (bBinary) ? std::ios::in | std::ios::binary : std::ios::in);
			m_bBinary = bBinary;	// バイナリフラグ保存
			m_filename = sPath;		// ファイル名を保存
			m_size = std::filesystem::file_size(sPath);		// ファイルサイズ取得
			return Get<std::ifstream>().is_open();
		}
		else
		{ // ファイルが見つからなかった為失敗
			return false;
		}
	}

	std::ios::openmode openmode = std::ios::out;		// ofstreamのフラグ

	switch (mode)
	{
	case MODE::WRITE:		// 書き込み (新規作成)
		openmode |= std::ios::trunc;	// 上書きフラグを立てる
		break;

	case MODE::WRITE_IF:	// 書き込み (作成済みなら取り消し)
		if (bFound == true) return false;		// もし見つかっているならここで終了
		openmode |= std::ios::trunc;	// 上書きフラグを立てる
		break;

	case MODE::APPEND:		// 書き込み (追記)
		openmode |= std::ios::app;		// 追記フラグを立てる
		m_size = (bFound == true) ? std::filesystem::file_size(sPath) : 0;		// ファイルサイズ取得
		break;

	default:
		break;
	}

	// 出力ストリームとして生成
	m_fstream.emplace<std::ofstream>(sPath, (bBinary) ? openmode | std::ios::binary : openmode);
	m_bBinary = bBinary;	// バイナリフラグ保存
	m_filename = sPath;		// ファイル名を保存
	return Get<std::ofstream>().is_open();
}

//==================================================================================
// --- ファイルクローズ処理 ---
//==================================================================================
void CFileStream::Close(void)
{
	m_fstream = std::monostate{};		// ファイルを閉じる
	m_filename.clear();		// ファイル名をリセット
	m_size = 0;				// ファイルサイズをリセット
}

//==================================================================================
// --- ファイルのオープン確認処理 ---
//==================================================================================
bool CFileStream::IsOpen(void) const 
{ // 空なら失敗
	if (IsState(STATE::INVALID)) return false;
	
	if (IsState(STATE::READ))
	{ // 入力ストリーム
		return Get<std::ifstream>().is_open();
	}
	else
	{ // 出力ストリーム
		return Get<std::ofstream>().is_open();
	}
}

//==================================================================================
// --- ファイルの状態確認処理 ---
//==================================================================================
bool CFileStream::IsState(const STATE state) const
{
	switch (state)
	{
	case STATE::INVALID:	// 無効値の確認
		return std::holds_alternative<std::monostate>(m_fstream);

	case STATE::READ:		// 読み込みモードかの確認
		return std::holds_alternative<std::ifstream>(m_fstream);

	case STATE::WRITE:		// 書き込みモードかの確認
		return std::holds_alternative<std::ofstream>(m_fstream);

	default:	// それ以外の場合失敗
		return false;
	}
}

//==================================================================================
// --- テキストファイルからの1行読み込み処理 ---
//==================================================================================
std::optional<std::string> CFileStream::ReadLine(void)
{ // 入力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::READ) == false || m_bBinary == true) return std::nullopt;

	// 1行読み取り、読み取れた場合読み取った文字列を返す
	// 読み取れ無かった場合は無効値を返す
	std::string line;		// 読み取った文字列
	if(!(std::getline(Get<std::ifstream>(), line))) return std::nullopt;
	return line;
}

//==================================================================================
// --- テキストファイルからの1文読み込み処理 ---
//==================================================================================
std::optional<std::string> CFileStream::ReadWord(void)
{ // 入力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::READ) == false || m_bBinary == true) return std::nullopt;

	// 1文読み取り、読み取った文字列を返す
	// 読み取れ無かった場合は無効値を返す
	std::string line;		// 読み取った文字列
	if (!(Get<std::ifstream>() >> line)) return std::nullopt;
	return line;
}

//==================================================================================
// --- テキストファイルからの特定の文字までの読み込み処理 ---
//==================================================================================
std::optional<std::string> CFileStream::ReadUntil(const char cDelim)
{ // 入力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::READ) == false || m_bBinary == true) return std::nullopt;

	// 特定の文字まで読み取り、読み取った文字列を返す
	// 読み取れ無かった場合は無効値を返す
	std::string line;		// 読み取った文字列
	if (!(std::getline(Get<std::ifstream>(), line, cDelim))) return std::nullopt;
	return line;
}

//==================================================================================
// --- テキストファイルへの文字列の書き出し処理 ---
//==================================================================================
bool CFileStream::WriteString(std::string_view out)
{ // 出力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::WRITE) == false || m_bBinary == true) return false;

	// ストリーム演算子で書き出し
	Get<std::ofstream>() << out;
	return static_cast<bool>(Get<std::ofstream>());
}

//==================================================================================
// --- バイナリファイルからの読み込み処理 ---
//==================================================================================
bool CFileStream::ReadBytes(std::span<std::byte> in)
{ // ストリームもしくはフラグが無効なら失敗
	if (IsState(STATE::READ) == false || m_bBinary == false) return false;

	size_t byte = in.size_bytes();		// 読み込むバイト数を取得
	char *pIn = reinterpret_cast<char*>(in.data());		// 引数の先頭ポインタをchar*として取得

	// バイナリデータを読み込み
	Get<std::ifstream>().read(pIn, byte);
	return static_cast<bool>(Get<std::ifstream>());
}

//==================================================================================
// --- バイナリファイルへの書き込み処理 ---
//==================================================================================
bool CFileStream::WriteBytes(std::span<const std::byte> out)
{ // 出力ストリーム以外もしくはフラグが無効の場合、無効値を返す
	if (IsState(STATE::WRITE) == false || m_bBinary == false) return false;

	size_t byte = out.size_bytes();		// 書き込むバイト数を取得
	const char *pOut = reinterpret_cast<const char*>(out.data());		// 引数の先頭ポインタをconst char*として取得

	// バイナリデータを書き出し
	Get<std::ofstream>().write(pOut, byte);
	return static_cast<bool>(Get<std::ofstream>());
}

//==================================================================================
// --- EOFの確認処理 ---
//==================================================================================
bool CFileStream::IsEoF(void) const
{ // ファイルのオープン確認
	if (std::holds_alternative<std::ifstream>(m_fstream) == false)
	{ // 入力ストリームでファイルが開かれていなければ、失敗扱い
		return false;
	}

	// ファイルのEOFを返す
	return Get<std::ifstream>().eof();
}