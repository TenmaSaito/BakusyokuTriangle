//==================================================================================
// 
// アイテム取得時の噛みつき演出クラスのヘッダーファイル [itemBiteEffect.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _ITEM_BITE_EFFECT_H_
#define _ITEM_BITE_EFFECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectBillboard3D;

//**********************************************************************************
// *** アイテム取得時の噛みつき演出クラス ***
//**********************************************************************************
class CItemBiteEffect : public CObject
{
public:
	CItemBiteEffect(const int nPriority = ITEM_BITE_EFFECT_PRIORITY);
	~CItemBiteEffect();

	static CItemBiteEffect *Create(const Vector3 &pos, const float fWidth, const int nTime, const Matrix *pMtxParent);
	HRESULT Init(const Vector3 &pos, const float fWidth, const int nTime, const Matrix *pMtxParent);
	void Uninit(void) override;
	void Update(void) override;

private:
	static constexpr int ITEM_BITE_EFFECT_PRIORITY = DEFAULT_EFFECT_PRIORITY;	// 描画・更新順位

	void UpdatePose(void);	// 牙の位置と透明度を更新

	CObjectBillboard3D *m_pUpper = nullptr;	// 上あご
	CObjectBillboard3D *m_pLower = nullptr;	// 下あご
	Vector3 m_pos = VECTOR3_NULL;	// 発生位置 (ワールド座標)
	float m_fWidth = 0.0f;		// 牙の横幅 (ワールド単位)
	int m_nTime = 0;			// 演出時間 (フレーム数)
	int m_nElapsed = 0;			// 経過フレーム数
	bool m_bEnded = false;		// 終了処理済みフラグ
};
#endif