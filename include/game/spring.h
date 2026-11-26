//==================================================================================
//
// バネクラスのヘッダファイル [spring.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _SPRING_H_		// インクルードガード
#define _SPRING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/objectX.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;
class CStage;

//**********************************************************************************
// *** バネクラス ***
//**********************************************************************************
class CSpring : public CObjectX
{
public:
	CSpring(const int nPriority = SPRING_PRIORITY);
	~CSpring();

	static CSpring *Create(const Vector3 &pos,
		const Vector3 &vec,
		const float fPower,
		const float fRadius);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &vec,
		const float fPower,
		const float fRadius);
	void Uninit(void) override;
	void Update(const ManagerContext::RenderContext &context) override;
	void Draw(const ManagerContext::RenderContext &context) override;

private:
	static constexpr int SPRING_PRIORITY = DEFAULT_OBJ_PRIORITY;		// バネの優先順位
	CPlayer *m_pPlayer = nullptr;		// プレイヤー
	CStage *m_pStage = nullptr;			// ステージ
	Vector3 m_vec = VECTOR3_NULL;		// 跳ねる向き
	float m_fPower = 0.0f;		// 跳ねる強さ
	float m_fRadius = 0.0f;		// 当たり判定の半径
};
#endif