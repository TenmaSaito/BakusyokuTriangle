//==================================================================================
//
// 食べ物の背景演出クラスのヘッダーファイル [foodBackground.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _FOOD_BACKGROUND_H_		// インクルードガード
#define _FOOD_BACKGROUND_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int FOOD_BACKGROUND_PRIORITY = DEFAULT_BG_PRIORITY;	// 描画・更新順位
constexpr int FOOD_BACKGROUND_COLUMN = 7;						// 横の配置数
constexpr int FOOD_BACKGROUND_ROW = 4;							// 縦の配置数
constexpr int FOOD_BACKGROUND_COUNT = FOOD_BACKGROUND_COLUMN * FOOD_BACKGROUND_ROW;	// 配置数
constexpr int FOOD_BACKGROUND_MODEL_COUNT = 3;					// モデルの種類数

//**********************************************************************************
// *** 食べ物の背景演出クラス ***
//
// INFO : 食べ物モデルを画面全体にまばらに配置する装飾用オブジェクト
//        カメラに追従し、大きさや向きはランダムにばらつかせる。当たり判定は持たない
//**********************************************************************************
class CFoodBackground : public CObject
{
public:
	CFoodBackground(const int nPriority = FOOD_BACKGROUND_PRIORITY);
	~CFoodBackground();

	static CFoodBackground *Create(void);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	//**********************************************************************************
	// *** 装飾ごとの描画情報 ***
	//**********************************************************************************
	struct Decoration
	{
		Matrix mtxWorld;					// ワールドマトリックス
		Vector3 rotOffset = VECTOR3_NULL;	// 回転のずらし量
		Vector2 posOffset = VECTOR2_NULL;	// 配置位置のずれ (マス単位)
		float fSizeRate = 1.0f;				// 大きさの倍率
		int nModel = 0;						// モデルの種類
	};

	std::array<Decoration, FOOD_BACKGROUND_COUNT> m_aDecoration = {};	// 装飾情報
	std::array<UINT, FOOD_BACKGROUND_MODEL_COUNT> m_aModelId = {};		// 登録済みモデルのインデックス
	float m_fAngle = 0.0f;			// 回転角
	bool m_bReady = false;			// 描画可能フラグ
	bool m_bPoseValid = false;		// 配置の計算済みフラグ
	bool m_bEnded = false;			// 終了処理済みフラグ
};
#endif
