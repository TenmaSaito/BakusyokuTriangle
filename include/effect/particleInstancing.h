//==================================================================================
//
// パーティクル(インスタンシング描画)クラスのヘッダファイル [particleInstancing.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _PARTICLE_INSTANCING_H_		// インクルードガード
#define _PARTICLE_INSTANCING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "core/resourceable.h"

//**********************************************************************************
// *** パーティクル(インスタンシング描画)クラス ***
//**********************************************************************************
class CParticleInstancing : public CObject
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

	CParticleInstancing(const int nPriority = INSTANCING_PRIORITY);
	~CParticleInstancing();

	static CParticleInstancing *Create(const Setting &setting, const int nMaxEffect);

	HRESULT Init(const Setting &setting, const int nMaxEffect);
	void Uninit(void) override;
	void Update(const ManagerContext::RenderContext &context) override;
	void Draw(const ManagerContext::RenderContext &context) override;
	void Set(const Vector3 &pos,
		const Vector3 &move,
		const Color &col,
		const float fRadius,
		const float fRadiusDecrease,
		const int nLife);
	void OnLostDevice(void) override;
	void OnResetDevice(void) override;
	void BindShader(const int nIdxShader) { m_nIdxShader = nIdxShader; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	int GetNumEffect(void) const { return m_vInstData.size(); }

private:
	// 共有するポリゴン情報
	struct VERTEX_GEOMETRY
	{
		Vector3 pos;	// 頂点座標
		Vector2 tex;	// テクスチャ座標
	};

	// インスタンス毎に異なる情報
	struct VERTEX_INSTANCE
	{
		Vector3 pos;	// ワールド座標
		float fRadius;	// 半径
		D3DCOLOR col;	// 色
	};

	// CPUで持つデータ構造
	struct InstanceData
	{
		Vector3 pos = VECTOR3_NULL;		// 位置
		Vector3 move = VECTOR3_NULL;	// 移動量
		Color col = COLOR_NULL;			// 色
		float fRadiusDecrease = 0.0f;	// 一フレーム毎の減少半径
		float fRadius = 0.0f;		// 半径
		int nLife = 0;				// 体力 (残りフレーム数)
	};

	static constexpr int INSTANCING_PRIORITY = DEFAULT_ADD_PRIORITY;		// インスタンシングエフェクトの優先順位
	static constexpr int MAX_INSTANCING = 100000000;		// インスタンシング描画の上限数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;			// 頂点バッファ
	LPDIRECT3DVERTEXBUFFER9 m_pInstBuff = nullptr;			// インスタンスバッファ
	LPDIRECT3DVERTEXDECLARATION9 m_pVtxDeclBuff = nullptr;	// デクレ―ションバッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff = nullptr;			// インデックスバッファ
	const Matrix *m_pMtxParent = nullptr;		// 親マトリックスへのポインタ
	std::vector<InstanceData> m_vInstData;		// インスタンスデータ配列
	Setting m_setting = {};		// パーティクル生成の設定情報
	int m_nMaxEffect = 0;		// 出現するエフェクトの最大数
	int m_nIdxShader = IResourceable::INVALID_RESOURCE_ID;		// シェーダーインデックス
	int m_nIdxTexture = IResourceable::INVALID_RESOURCE_ID;		// テクスチャインデックス
};
#endif