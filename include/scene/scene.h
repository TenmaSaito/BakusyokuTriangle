//==================================================================================
// 
// シーンクラスのヘッダーファイル [scene.h]
// Author : TENMA SAITO
// Date   : 2026/6/24
// 
//==================================================================================
#ifndef _SCENE_H_		// インクルードガード
#define _SCENE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"
#include <concepts>

namespace ManagerContext
{
	struct SceneContext;
}

//**********************************************************************************
// *** シーンクラス ***
//**********************************************************************************
class CScene
{
public:
	// モードの種類
	enum class MODE
	{
		NONE = 0,		// 無し
		TITLE,			// タイトルシーン
		GAME,			// ゲームシーン
		RANKING,		// ランキングシーン
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const MODE value) noexcept
	{
		return EnumUtil::InRange(value, MODE::TITLE, MODE::MAX);
	}

	CScene(const MODE mode);
	virtual ~CScene();

	static CScene *Create(const ManagerContext::SceneContext &sceneContext, 
		const MODE mode,
		std::unique_ptr<CScene> &rpOut);

	virtual HRESULT Init(const ManagerContext::SceneContext &sceneContext) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(const ManagerContext::SceneContext &sceneContext) = 0;
	virtual void Draw(const ManagerContext::SceneContext &sceneContext) = 0;
	MODE GetMode(void) const { return m_mode; }

private:
	MODE m_mode = MODE::NONE;		// 現在のモード
};

//**********************************************************************************
// *** シーンのモード取得用CRTPクラス ***
//**********************************************************************************
template<class Derived, CScene::MODE myMode>
class CSceneBase : public CScene
{
public:
	CSceneBase() : CScene(myMode) {}
	static constexpr MODE GetOwnMode(void) { return myMode; }
};

// クラスがシーンの条件を満たしているかの条件
template<class T>
concept IsScene = std::derived_from<T, CScene> &&
requires (T t)
{ // 自クラスがどのモードに属しているかを取得する関数
	T::GetOwnMode();
};
#endif