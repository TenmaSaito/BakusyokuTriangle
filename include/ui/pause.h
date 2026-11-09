//==================================================================================
// 
// ポーズ画面クラスのヘッダーファイル [pause.h]
// Author : TENMA SAITO
// Date   : 2026/9/3
// 
//==================================================================================
#ifndef _PAUSE_H_		// インクルードガード
#define _PAUSE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** ポーズクラス ***
//**********************************************************************************
class CPause
{
public:
	// 現在選ばれている選択肢の種類
	enum class TYPE
	{
		CONTINUE = 0,		// 続行
		RESTART,			// やり直し
		EXIT,				// 終了
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const TYPE value) noexcept
	{
		return EnumUtil::InRange(value, TYPE::CONTINUE, TYPE::MAX);
	}

	CPause();
	virtual ~CPause();

	static CPause *Create(void);

	HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	TYPE GetType(void) const { return m_type; }

protected:
	const CPolygon2D *GetMenu(const TYPE type) const;
	void ExecuteType(const TYPE type);

private:
	void UpdateInput(void);
	virtual void ActionType(void);

	std::array<std::unique_ptr<CPolygon2D>, EnumUtil::ToIndex(TYPE::MAX)> m_apMenu;		// 選択肢ポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pBack;		// 背景ポリゴン
	TYPE m_type;		// 現在のタイプ
	bool m_bDisp;		// 描画フラグ
};
#endif