//==================================================================================
//
// パーティクル(バッチ描画)クラスのヘッダファイル [particleBatch.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _PARTICLE_BATCH_H_		// インクルードガード
#define _PARTICLE_BATCH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** パーティクル(バッチ描画)クラス ***
//**********************************************************************************
class CParticleBatch : public CObject
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

	CParticleBatch(const int nPriority = BATCH_PRIORITY);
	~CParticleBatch();

	HRESULT Init(const Setting &setting, const int nMaxEffect);

private:
	// バッチ描画する各オブジェクトの情報
	struct Object
	{
		Vector3 pos = VECTOR3_NULL;			// 位置
		Vector2 size = VECTOR2_NULL;		// サイズ
		Color col = COLOR_ONE;				// 色
	};

	void InitVertex(VERTEX_3D *pVtx, const int nMaxEffect);
	void InitIndex(WORD *pItx);

	static constexpr int BATCH_PRIORITY = DEFAULT_ADD_PRIORITY;		// バッチエフェクトの優先順位
	static constexpr int MAX_BATCH = 15000;				// バッチ描画の最大数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;		// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff = nullptr;		// インデックスバッファ
	std::vector<Object> m_vBatchInst;		// バッチ描画のインスタンス
	Setting m_setting = {};					// 設定
};
#endif