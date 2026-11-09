//==================================================================================
// 
// インプットシステムクラスのヘッダーファイル [inputSystem.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _INPUT_SYSTEM_H_		// インクルードガード
#define _INPUT_SYSTEM_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "core/singleton.h"
#include <vector>
#include <memory>
#include <string_view>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CInputAction;
class CInputActionMap;
class CInputKeyboard;
class CInputMouse;
class CJoypad;
struct INPUT_CONTROL_PATH;

//**********************************************************************************
// *** インプットシステムクラス ***
//**********************************************************************************
class CInputSystem : public ISingleton<CInputSystem>
{
public:
    HRESULT Init(const HINSTANCE hInstance, 
        const HWND hWnd);
    void Uninit(void);
    void Update(void);

    CInputActionMap *CreateActionMap(std::string_view sName);
    CInputAction *FindAction(std::string_view sMapName,
        std::string_view sActionName) const;
    float ReadControl(const INPUT_CONTROL_PATH &path) const;
    Vector3 ReadStick(const INPUT_CONTROL_PATH &path) const;
    CInputKeyboard *GetKeyboard(void) const { return m_pKeyboard.get(); }
    CInputMouse *GetMouse(void) const { return m_pMouse.get(); }
    CJoypad *GetJoypad(void) const { return m_pJoypad.get(); }

private:
    CInputSystem();
    ~CInputSystem();

    std::unique_ptr<CInputKeyboard> m_pKeyboard;	// キーボード
    std::unique_ptr<CInputMouse> m_pMouse;			// マウス
    std::unique_ptr<CJoypad> m_pJoypad;				// ジョイパッド
    std::vector<std::unique_ptr<CInputActionMap>> m_vpActionMap; 	// インプットアクションのマップの配列

    friend ISingleton<CInputSystem>;        // シングルトン経由の生成のみ許可
};
#endif