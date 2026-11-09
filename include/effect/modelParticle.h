//==================================================================================
//
// モデルパーティクルクラスのヘッダーファイル [modelParticle.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _MODEL_PARTICLE_H_		// インクルードガード
#define _MODEL_PARTICLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "render/xfile.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CStage;

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int MODEL_PARTICLE_PRIORITY = DEFAULT_EFFECT_PRIORITY;	// 描画・更新順位

//**********************************************************************************
// *** モデルパーティクルクラス ***
//**********************************************************************************
class CModelParticle : public CObject
{
public:
	// パーティクル生成の設定情報
	struct Setting
	{
		std::string sModelPath;					// モデルのファイルパス
		Vector3 pos = VECTOR3_NULL;				// 発生位置 (ステージ上のローカル座標)
		Vector3 move = VECTOR3_NULL;			// 初速
		float fScale = 1.0f;					// 大きさ
		float fGravity = 0.5f;					// 重力の強さ
		float fBounce = 0.4f;					// 床で跳ね返るときの反発係数 (0で跳ねない)
		int nNum = 0;							// 一度に出現させるモデルの数
		int nLife = 0;							// 持続フレーム数

		// ExSettings
		Vector3 posVariation = VECTOR3_NULL;	// 発生位置のぶれの範囲 (X >= 0, Y >= 0, Z >= 0)
		Vector3 moveVariation = VECTOR3_NULL;	// 初速のぶれの範囲 (X >= 0, Y >= 0, Z >= 0)
		float fScaleVariation = 0.0f;			// 大きさのぶれの範囲 (X >= 0)
		float fRotSpeedMax = 0.0f;				// 回転速度の最大値 (ラジアン / フレーム)
		int nLifeVariation = 0;					// 持続フレーム数のぶれの範囲 (X >= 0)
	};

	CModelParticle(const int nPriority = MODEL_PARTICLE_PRIORITY);
	~CModelParticle();

	static CModelParticle *Create(const Setting &setting);

	HRESULT Init(const Setting &setting);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Notified(CObject *pObject, std::string_view message, const std::any &args) override;

private:
	//**********************************************************************************
	// *** モデル1個ごとの情報 ***
	//**********************************************************************************
	struct Piece
	{
		Matrix mtxWorld;					// ワールドマトリックス
		Vector3 pos = VECTOR3_NULL;			// 位置 (ステージ上のローカル座標)
		Vector3 move = VECTOR3_NULL;		// 移動量
		Vector3 rot = VECTOR3_NULL;			// 角度
		Vector3 rotMove = VECTOR3_NULL;		// 回転速度
		float fScale = 1.0f;				// 大きさ
		int nLife = 0;						// 残りフレーム数
	};

	const Matrix *GetStageMatrix(void);

	CStage *m_pStage = nullptr;				// 親にするステージへのポインタ
	std::vector<Piece> m_vPiece;			// モデル1個ごとの情報
	UINT m_uModelId = INVALID_XFILE_ID;		// 登録済みモデルのインデックス
	Setting m_setting = {};					// 設定
};
#endif
