//==================================================================================
// 
// 紐ずけテーブルの読み込みクラスのヘッダーファイル [inputKeytable.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _INPUT_KEYTABLE_H_		// インクルードガード
#define _INPUT_KEYTABLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/singleton.h"
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

//**********************************************************************************
// *** 紐ずけテーブルの読み込みクラス ***
//**********************************************************************************
class CInputKeyTable : public ISingleton<CInputKeyTable>
{
    friend ISingleton<CInputKeyTable>;      // インスタンス生成をシングルトンに限定

public:
    HRESULT LoadFromFile(std::string_view sFilePath, const HWND hWnd);
    bool FindCode(std::string_view sName, int &nCodeOut) const;      // 名前 → コード (大文字化して検索)
    const std::string *FindName(const int nCode) const;              // コード → 正式名

private:
    CInputKeyTable() = default;
    ~CInputKeyTable() = default;

    void Parse(std::string sString, std::vector<std::string> &rOut);

    std::unordered_map<std::string, int> m_mapNameToCode;
    std::unordered_map<int, std::string> m_mapCodeToName;            // 最初に出た名前だけ登録
};
#endif