//==================================================================================
// 
// アイテムクラスのヘッダーファイル [item.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _ITEM_H_		// インクルードガード
#define _ITEM_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/objectX.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;
class CParticle;
class CNumber3D;
class CObjectBillboard3D;
class CScore;

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int ITEM_PRIORITY = DEFAULT_OBJ_PRIORITY;		// アイテムの優先順位

//**********************************************************************************
// *** アイテムクラス ***
//**********************************************************************************
class CItem : public CObjectX
{
public:
	// アイテム(食べ物)の種類
	enum class TYPE
	{
		NONE = 0,	// 指定なし
		RICE,		// ご飯
		SIDE_DISH,	// おかず
		SOUP,		// 汁物
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const TYPE value) noexcept
	{
		return EnumUtil::InRange(value, TYPE::RICE, TYPE::MAX);
	}

	CItem(const int nPriority = ITEM_PRIORITY);
	~CItem();

	static CItem *Create(const Vector3 &pos,
		const float fRadius,
		const int nLife,
		const TYPE type);

	HRESULT Init(const Vector3 &pos,
		const float fRadius,
		const int nLife,
		const TYPE type);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool IsEqualType(const TYPE type) const { return m_type == type; }

private:
	CObjectBillboard3D *m_pIcon = nullptr;		// 表示アイコン
	CPlayer *m_pPlayer = nullptr;		// プレイヤーへのポインタ
	CParticle *m_pParticle = nullptr;	// パーティクルへのポインタ
	CScore *m_pScore = nullptr;			// スコア表示へのポインタ
	Vector3 m_move = VECTOR3_NULL;	// 加速度
	float m_fRadius = 0.0f;			// モデルの獲得半径
	int m_nLife = 0;				// 体力
	TYPE m_type = TYPE::NONE;		// 自身のタイプ
};
#endif