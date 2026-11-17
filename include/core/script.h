//==================================================================================
// 
// スクリプトクラスのヘッダーファイル [script.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _SCRIPT_H_		// インクルードガード
#define _SCRIPT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <memory>
#include <string>
#include <string_view>
#include <optional>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CFileStream;

//**********************************************************************************
// *** スクリプトクラス ***
//**********************************************************************************
class CScript
{
public:
	CScript();
	virtual ~CScript();

	virtual HRESULT Load(std::string_view path) = 0;

protected:
	bool Open(std::string_view path);
	void Close(void);
	std::optional<std::string> ReadCleanLine(void);
	bool SeekScriptBegin(void);
	bool IsScriptEnd(std::string_view line);

private:
	std::unique_ptr<CFileStream> m_pFile;		// ファイルストリーム
};
#endif