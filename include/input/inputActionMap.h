//==================================================================================
// 
// インプットアクションのマップクラスのヘッダーファイル [inputActionMap.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _INPUT_ACTION_MAP_H_		// インクルードガード
#define _INPUT_ACTION_MAP_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "input/inputAction.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CInputSystem;

//**********************************************************************************
// *** インプットアクションのマップクラス ***
//**********************************************************************************
class CInputActionMap
{
public:
    CInputActionMap();
    ~CInputActionMap();

	HRESULT Init(std::string_view sName);
	void Uninit(void);
    void Update(const CInputSystem &inputSystem);

    CInputAction *Add(std::string_view sName,
        const CInputAction::ACTION_TYPE type,
        const float fPressPoint = CInputAction::GetDefaultPressPoint());
    CInputAction *Find(std::string_view sName) const;
    void SetEnable(const bool bEnable);
    bool IsEqualName(std::string_view sName) const { return m_sName == sName; }
    
private:
	std::string m_sName;		// アクションマップの名前
    bool m_bEnable = true;      // 有効フラグ
    std::vector<std::unique_ptr<CInputAction>> m_vpAction;      // インプットアクションの配列
};
#endif