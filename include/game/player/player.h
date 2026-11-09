//==================================================================================
// 
// プレイヤークラスのヘッダーファイル [player.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/3d/objectX.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CStage;

//**********************************************************************************
// *** プレイヤークラス ***
//**********************************************************************************
class CPlayer : public CObjectX
{
public:
	CPlayer(int nPriority = PLAYER_PRIORITY);
	~CPlayer();

	static CPlayer *Create(std::string_view pFilename,
		const Vector3 &pos,
		const float fRadius);

	HRESULT Init(std::string_view pFilename,
		const Vector3 &pos,
		const float fRadius);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Notified(CObject *pObject, std::string_view message, const std::any &args) override;
	void AddForce(const float fRotDest, const float fValueRot);
	const Vector3 *GetPositionOld(void) const { return &m_posOld; }
	float GetRadius(void) const { return m_fRadius * m_fScaling; }
	bool IsAutoRotating(void) const { return m_bAutoRotating; }
	void Eat(void);

	static CPlayer *Find(void);	// シーン上のインスタンスを検索

private:
	static constexpr int PLAYER_PRIORITY = DEFAULT_OBJX_PRIORITY;		// プレイヤーの優先順位

	void CharactorMovable(void);
	void StatusChange(void);

	CStage *m_pStage = nullptr;			// ステージへのポインタ
	Vector3 m_posOld = VECTOR3_NULL;	// 過去の位置
	Vector3 m_move = VECTOR3_NULL;		// 移動量
	Vector3 m_rotDest = VECTOR3_NULL;	// 目標角度
	Vector3 m_scl = VECTOR3_ONE;		// スケーリング倍率
	Matrix m_mtxRot;					// 回転マトリックス (保存用)
	Quaternion m_quat;					// クォータニオン
	Vector3 m_vecAxis = VECTOR3_NULL;	// 回転軸
	float m_fValueRot = 0.0f;			// 回転角
	float m_fAngle = 0.0f;				// 転がる角度
	float m_fAngleAdd = 0.0f;			// 角度の増加値
	float m_fRotSpeed = 0.0f;			// 回転速度
	float m_fRadius = 0.0f;				// 半径
	float m_fScaling = 0.0f;			// 拡大倍率
	bool m_bAutoRotating = true;		// 自動回転中か
};
#endif