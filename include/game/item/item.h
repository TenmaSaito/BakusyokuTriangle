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
// *** マクロ定義 ***
//**********************************************************************************
#define ENABLE_ORDER
#undef ENABLE_ORDER

#define ENABLE_ALL
#undef ENABLE_ALL

#define ENABLE_LOOP
#if defined(ENABLE_ORDER) || defined(ENABLE_ALL)
#undef ENABLE_LOOP
#endif

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;
class CParticle;
class CNumber3D;
class CScore;
class CShadowSt;
class CMeshRing;

//**********************************************************************************
// *** アイテムクラス ***
//**********************************************************************************
class CItem : public CObjectX
{
public:
#if defined(ENABLE_LOOP) || defined(ENABLE_ALL)
	// アイテム(食べ物)の種類
	enum class TYPE
	{
		NONE = 0,	// 指定なし
		RICE,		// ご飯
		SIDE_DISH,	// おかず
		SOUP,		// 汁物
		MAX
	};
#elif defined(ENABLE_ORDER)
	// アイテム(食べ物)の種類
	enum class TYPE
	{
		NONE = 0,	// 指定なし
		T_0,
		T_1,
		T_2,
		T_3,
		T_4,
		T_5,
		T_6,
		T_7,
		T_8,
		T_9,
		MAX
	};
#endif

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
	void Update(const ManagerContext::RenderContext &context) override;
	void Draw(const ManagerContext::RenderContext &context) override;
	bool IsEqualType(const TYPE type) const { return m_type == type; }
	TYPE GetItemType(void) const { return m_type; }		// CObject::GetType を隠さないよう別名にする
	static constexpr int GetTypeMax(void) { return static_cast<int>(TYPE::MAX) - 1; }
	static constexpr int ConvertToArrayIndex(const CItem::TYPE type) { return static_cast<int>(type) - 1; }

private:
	static constexpr int ITEM_PRIORITY = DEFAULT_OBJ_PRIORITY;		// アイテムの優先順位

	CShadowSt *m_pShadowSt = nullptr;	// ステンシルシャドウへのポインタ
	CPlayer *m_pPlayer = nullptr;		// プレイヤーへのポインタ
	CParticle *m_pParticle = nullptr;	// パーティクルへのポインタ
	CMeshRing *m_pRing = nullptr;		// メッシュリング
	Vector3 m_move = VECTOR3_NULL;	// 加速度
	float m_fRadius = 0.0f;			// モデルの獲得半径
	float m_fRotate = 0.0f;			// 回転角度
	int m_nLife = 0;				// 体力
	TYPE m_type = TYPE::NONE;		// 自身のタイプ
};
#endif