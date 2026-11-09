//==================================================================================
// 
// オブジェクトクラスのヘッダーファイル [object.h]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
#ifndef _OBJECT_H_		// インクルードガード
#define _OBJECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"
#include <any>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEBUG_ASSERT_TYPE_NONE				// オブジェクトにタイプが指定されていなかった場合、アサーション

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int MAX_OBJPRIORITY = 8;			// 優先順位の総数
constexpr int DEFAULT_BG_PRIORITY = 0;		// 背景の優先順位
constexpr int DEFAULT_OBJ_PRIORITY = 3;		// オブジェクトの優先順位
constexpr int DEFAULT_EFFECT_PRIORITY = 4;	// エフェクト関連の優先順位
constexpr int DEFAULT_ADD_PRIORITY = 5;		// 加算合成関連の優先順位
constexpr int DEFAULT_UI_PRIORITY = 6;		// UI関連の優先順位
constexpr int FINALLY_PRIORITY = 7;			// 全てのオブジェクトが描画された後の最終優先順位
constexpr std::string_view NOTIFY_WHEN_DEATH = "Death";			// 死んだ際に通知される文字列

//**********************************************************************************
// *** オブジェクトクラス ***
//**********************************************************************************
class CObject
{
public:
	// オブジェクトの種類
	enum class TYPE
	{
		NONE = 0,		// 指定無し (アサーション対象)
		OBJ_2D,		// Object2D
		OBJ_3D,		// Object3D
		OBJ_BILLBOARD_3D,	// Object3D[Billboard]
		OBJ_X,			// ObjectX
		OBJ_X_AFTERIMAGE,	// ObjectXの残像
		OBJ_LINE,		// ObjectLine
		FIELD,			// 床
		WALL,			// 壁
		PLAYER,		// プレイヤー
		SHADOW,		// 影
		ITEM,			// アイテム
		STAGE,			// ステージ
		TIMER,			// タイマー
		PARTICLE,		// パーティクル
		ITEM_GENERATOR,	// アイテム生成機
		SCORE,			// スコア
		SCREEN_EFFECT,	// 画面エフェクト (閃光)
		NEXTINFO,		// 次のアイテムの表示
		MODEL_PARTICLE,	// モデルパーティクル
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const TYPE value) noexcept
	{
		return EnumUtil::InRange(value, TYPE::OBJ_2D, TYPE::MAX);
	}

	CObject(const int nPriority = DEFAULT_OBJ_PRIORITY);
	virtual ~CObject();

	virtual HRESULT Init(void) { return S_OK; }
	virtual void Uninit(void) {}
	virtual void Update(void) {}
	virtual void Draw(void) {}
	virtual void Notified(CObject *pObject, std::string_view message, const std::any &args = {}) {}

	void AddRenderState(const std::pair<D3DRENDERSTATETYPE, DWORD> state, const bool bAfter);
	void SetRenderState(const LPDIRECT3DDEVICE9 pDevice, const bool bAfter);
	bool IsValidRenderState(const bool bAfter) { return (bAfter == true) ? !m_vAfterRenderState.empty() : !m_vBeforeRenderState.empty(); }
	void NotifyAll(std::string_view message, const std::any &args = {});
	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) const { return m_type; }
	bool IsDeath(void) const { return m_bDeath; }
	void AddObserver(CObject *pObserve) { m_vpObserver.push_back(pObserve); }
	void RemoveObserver(CObject *pObserve) { m_vpObserver.erase(std::remove(m_vpObserver.begin(), m_vpObserver.end(), pObserve), m_vpObserver.end()); }
	static CObject* GetTop(const int nPriority) { return m_apTop[nPriority]; }
	CObject *GetNext(void) const { return m_pNext; }
	static int GetNumAll(void) { return m_nNumAll; }
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static void FlagCheckAll(void);
	static CObject *FindAnyObjectByType(const TYPE type);
	static CObject *FindAnyObjectByType(const int nPriority, const TYPE type);
	template<class T> static T *FindAnyObjectByType(void);
	template<class T> static T *FindAnyObjectByType(const int nPriority);
	static std::vector<CObject*> FindObjectsByType(const TYPE type);
	static std::vector<CObject*> FindObjectsByType(const int nPriority, const TYPE type);
	template<class T> static std::vector<T*> FindObjectsByType(void);
	template<class T> static std::vector<T*> FindObjectsByType(const int nPriority);
	static void SetEnablePriority(const int nPriority, const bool bEnable) { m_vEnablePriority[nPriority] = bEnable; }
	static bool GetEnablePriority(const int nPriority) { return m_vEnablePriority[nPriority]; }

protected:
	void Release(void);

private:
	void AddList(void);
	void RemoveList(void);
	void NotifyAllWhenDeath(void);

	inline static CObject *m_apTop[MAX_OBJPRIORITY] = { nullptr };		// 先頭オブジェクトへのポインタ
	inline static CObject *m_apCur[MAX_OBJPRIORITY] = { nullptr };		// 最後尾オブジェクトへのポインタ
	inline static int m_nNumAll = 0;	// オブジェクトの総数
	inline static std::vector<bool> m_vEnablePriority = std::vector<bool>(MAX_OBJPRIORITY, true);		// 各優先順位のフラグ
	std::vector<CObject*> m_vpObserver = {};	// 自身を管理しているオブジェクトへのポインタ
	CObject *m_pPrev = nullptr;			// 前のオブジェクトへのポインタ
	CObject *m_pNext= nullptr;			// 次のオブジェクトへのポインタ
	int m_nPriority = 0;			// 優先順位の位置
	TYPE m_type = TYPE::NONE;		// オブジェクトタイプ
	bool m_bDeath = false;			// 死亡フラグ
	std::vector<std::pair<D3DRENDERSTATETYPE, DWORD>> m_vBeforeRenderState;		// 描画前に設定されるレンダーステートの種類
	std::vector<std::pair<D3DRENDERSTATETYPE, DWORD>> m_vAfterRenderState;		// 描画後に設定されるレンダーステートの種類
};

//==================================================================================
// --- オブジェクトの検索処理 (タイプ指定 : 初めに見つかったポインタ) ---
//==================================================================================
template<class T> T *CObject::FindAnyObjectByType(void)
{
	T *pCast = nullptr;		// キャスト後のポインタ

	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 全プライオリティを検索
		pCast = FindAnyObjectByType<T>(nCntPriority);

		// ポインタがnullでなければ終了
		if (pCast != nullptr) break;
	}

	return pCast;
}

//==================================================================================
// --- オブジェクトの検索処理 (タイプ・優先順位指定 : 初めに見つかったポインタ) ---
//==================================================================================
template<class T> T *CObject::FindAnyObjectByType(const int nPriority)
{ // 範囲外ならアサーション
	assert(nPriority >= 0 && nPriority < MAX_OBJPRIORITY);

	CObject *pObject = m_apTop[nPriority];		// オブジェクトへのポインタ
	T *pCast = nullptr;		// キャスト後のポインタ

	while (pObject != nullptr)
	{ // オブジェクトへのポインタがnullではない間ループ
		CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ

		// タイプが同じだった場合、ループ終了
		pCast = dynamic_cast<T*>(pObject);
		if (pCast != nullptr) break;

		pObject = pObjectNext;		// ポインタを進める
	}

	// 検索結果を返す
	return pCast;
}

//==================================================================================
// --- オブジェクトの検索処理 (タイプ指定 : 見つかったポインタ全て) ---
//==================================================================================
template<class T> std::vector<T*> CObject::FindObjectsByType(void)
{
	std::vector<T*> vpObject;				// 見つかったオブジェクトへのポインタ

	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 全プライオリティを検索
		std::vector<T*> vpFind;		// 今回のプライオリティで見つかった配列

		// 検索
		vpFind = FindObjectsByType<T>(nCntPriority);

		// 配列の要素を挿入
		vpObject.insert(vpObject.end(), vpFind.begin(), vpFind.end());
	}

	// 検索結果を返す
	return vpObject;
}

//==================================================================================
// --- オブジェクトの検索処理 (優先順位, タイプ指定 : 見つかったポインタ全て) ---
//==================================================================================
template<class T> std::vector<T*> CObject::FindObjectsByType(const int nPriority)
{ // 範囲外ならアサーション
	assert(nPriority >= 0 && nPriority < MAX_OBJPRIORITY);

	CObject *pObject = m_apTop[nPriority];		// オブジェクトへのポインタ
	std::vector<T*> vpObject;					// 見つかったオブジェクトへのポインタ

	while (pObject != nullptr)
	{ // オブジェクトへのポインタがnullではない間ループ
		CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ

		// タイプが同じだった場合、ポインタを追加
		T *pCast = dynamic_cast<T*>(pObject);
		if (pCast != nullptr) vpObject.push_back(pCast);

		pObject = pObjectNext;		// ポインタを進める
	}

	// 検索結果を返す
	return vpObject;
}
#endif