//==================================================================================
// 
// パーティクルクラスのヘッダーファイル [particle.h]
// Author : TENMA SAITO
// Date   : 2026/8/24
// 
//==================================================================================
#ifndef _PARTICLE_H_		// インクルードガード
#define _PARTICLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** パーティクルクラス ***
//**********************************************************************************
class CParticle : public CObject
{
public:
	// パーティクル生成の設定情報
	struct Setting
	{
		Vector3 pos = VECTOR3_NULL;				// 発生位置
		Vector3 move = VECTOR3_NULL;			// 加速度
		Vector2 scale = VECTOR2_NULL;			// サイズ
		Color color = COLOR_NULL;				// パーティクルの色
		int nNumEffectFrame = 0;				// 一フレームに出現させるエフェクトの数
		int nLife = 0;							// 持続フレーム数
		bool bEnableAlphaBlending = false;		// αブレンディングフラグ

		// ExSettings
		Vector3 posVariation = VECTOR3_NULL;		// 発生位置のぶれの範囲 (X >= 0, Y >= 0, Z >= 0)
		Vector3 moveVariation = VECTOR3_NULL;		// 移動量のぶれの範囲 (X >= 0, Y >= 0, Z >= 0)
		Vector2 scaleVariation = VECTOR2_NULL;		// サイズのぶれの範囲 (X >= 0, Y >= 0)
		int nNumEffectVariation = 0;	// 発生するエフェクトのぶれの範囲 (X >= 0)
		int nEffectLifeMax = 0;			// エフェクトの体力の最大値 (X < nLife)
		int nPercent = 0;				// エフェクトの発生する確率 (X > 0 && X <= 100)
	};

	CParticle(const int nPriority = DEFAULT_EFFECT_PRIORITY);
	~CParticle();

	static CParticle *Create(const Setting &setting);

	HRESULT Init(const Setting &setting);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	const Matrix *GetParent(void) const { return &m_mtxWorld; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetEnable(const bool bEnable) { m_bEnable = bEnable; }

private:
	int m_nIdxTexture = -1;					// テクスチャインデックス
	const Matrix *m_pMtxParent = nullptr;	// 親マトリックスへのポインタ
	Setting m_setting = {};		// 設定
	Matrix m_mtxWorld;			// ワールドマトリックス
	bool m_bEnable = false;		// 一時的に生成をやめるか
};
#endif