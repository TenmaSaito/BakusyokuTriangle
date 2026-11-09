//==================================================================================
// 
// 紐ずけテーブルの読み込みクラスのソースファイル [inputKeytable.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input/inputKeytable.h"
#include <fstream>

//==================================================================================
// --- ファイル読み込み処理 ---
//==================================================================================
HRESULT CInputKeyTable::LoadFromFile(std::string_view sFilePath, const HWND hWnd)
{
	std::ifstream file(sFilePath.data());		// ファイル
	if (file.is_open() == false)
	{ // ファイルオープン失敗
		std::string sError = "ファイルオープンに失敗しました。 ファイルパス : ";
		sError += sFilePath.data();
		MessageBox(hWnd, sError.c_str(), "Fileload Error", MB_ICONERROR);
		return E_FAIL;
	}

	while (1)
	{ // eofになるまで読み込み
		std::string sRead;		// 読み込んだ一列
		std::vector<std::string> vsParse;		// パース後の文字列

		// 一列読み取り
		std::getline(file, sRead);
		if (file.eof()) break;		// eofの場合終了

		// 一列をパース
		Parse(sRead, vsParse);

		std::string_view sName = vsParse[0];		// 名前
		int nCode = std::stoi(vsParse[1].data(), nullptr, HEX_BASE);		// 値

		// マップに登録
		m_mapNameToCode[std::string(sName)] = nCode;
		m_mapCodeToName[nCode] = sName;
	}

	return S_OK;
}

//==================================================================================
// --- 文字列->コード変換処理 ---
//==================================================================================
bool CInputKeyTable::FindCode(std::string_view sName, int &nCodeOut) const
{
	return false;
}

//==================================================================================
// --- 文字列<-コード変換処理 ---
//==================================================================================
const std::string *CInputKeyTable::FindName(const int nCode) const
{
	return nullptr;
}

//==================================================================================
// --- 文字列のパース処理 ---
//==================================================================================
void CInputKeyTable::Parse(std::string sString, std::vector<std::string> &rOut)
{
	auto offset = std::string::size_type(0);		// オフセット
	constexpr auto separator_length = std::string_view(",").size();

	while (1)
	{ 
		// セパレーターを検索
		auto pos = sString.find(",", offset);
		if (pos == std::string::npos)
		{ // 見つからなかった場合、直前の文字をpushして終了
			rOut.push_back(sString.substr(offset));
			break;
		}

		// セパレータの前の文字をpushして、オフセットをずらす
		rOut.push_back(sString.substr(offset, pos - offset));
		offset = pos + separator_length;
	}
}