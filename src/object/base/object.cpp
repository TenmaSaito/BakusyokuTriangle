//==================================================================================
// 
// オブジェクトクラスのソースファイル [object.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object/base/object.h"
#include "core/manager.h"
#include "render/camera.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObject::CObject(const int nPriority)
{ // 優先順位保存 (範囲外ならアサーション)
	assert(nPriority >= 0 && nPriority < MAX_OBJPRIORITY);
	m_nPriority = nPriority;
	
	// 自身をリストに追加
	AddList();

	// 総数増加
	m_nNumAll++;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObject::~CObject()
{
}

//==================================================================================
// --- レンダーステートの追加処理 ---
//==================================================================================
void CObject::AddRenderState(const std::pair<D3DRENDERSTATETYPE, DWORD> state, const bool bAfter)
{
	if (bAfter == false)
	{ // 描画前に設定するなら
		m_vBeforeRenderState.push_back(state);
	}
	else
	{ // 描画後に設定するなら
		m_vAfterRenderState.push_back(state);
	}
}

//==================================================================================
// --- レンダーステートの設定処理 ---
//==================================================================================
void CObject::SetRenderState(const LPDIRECT3DDEVICE9 pDevice, const bool bAfter)
{
	if (bAfter == false)
	{ // 描画前に設定するなら
		for (const auto &state : m_vBeforeRenderState)
		{ // 追加された順にステートを設定
			pDevice->SetRenderState(state.first, state.second);
		}
	}
	else
	{ // 描画後に設定するなら
		for (const auto &state : m_vAfterRenderState)
		{ // 追加された順にステートを設定
			pDevice->SetRenderState(state.first, state.second);
		}
	}
}

//==================================================================================
// --- 自身の観察者に対しての通知処理 ---
//==================================================================================
void CObject::NotifyAll(std::string_view message, const std::any &args)
{
	std::vector<CObject*> vpSnapshot = m_vpObserver;	// Notify直前のポインタ配列のコピー
	for (auto &pObserver : vpSnapshot)
	{ // nullの場合スキップ
		if (pObserver == nullptr) continue;

		// 通知
		pObserver->Notified(this, message, args);
	}
}

//==================================================================================
// --- 全オブジェクトの解放処理 ---
//==================================================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject *pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

			// オブジェクトの終了
			pObject->Uninit();

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}

	// 死亡フラグを確認
	FlagCheckAll();
}

//==================================================================================
// --- 全オブジェクトの更新処理 ---
//==================================================================================
void CObject::UpdateAll(void)
{
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		if (m_vEnablePriority[nCntPriority] == false) continue;

		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject *pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

#ifdef _DEBUG
#ifdef DEBUG_ASSERT_TYPE_NONE
			// オブジェクトのタイプがNONEの場合、アサーション + ブレークポイント
			assert(pObject->m_type != TYPE::NONE);
			if (pObject->m_type == TYPE::NONE) __debugbreak();
#endif
#endif
			// オブジェクトの更新
			pObject->Update();

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}

	// 死亡フラグを確認
	FlagCheckAll();
}

//==================================================================================
// --- 全オブジェクトの描画処理 ---
//==================================================================================
void CObject::DrawAll(void)
{
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		if (m_vEnablePriority[nCntPriority] == false) continue;

		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject* pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

			// オブジェクトの描画
			pObject->Draw();

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}
}

//==================================================================================
// --- 全オブジェクトのフラグ確認処理 ---
//==================================================================================
void CObject::FlagCheckAll(void)
{
	// 死亡フラグを確認
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject *pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

			if (pObject->m_bDeath == true)
			{ // 死亡フラグが立っていれば
				// リストから削除
				pObject->RemoveList();

				// 自分自身を破棄
				delete pObject;

				m_nNumAll--;		// オブジェクトの総数を減らす
			}

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}
}

//==================================================================================
// --- オブジェクトの検索処理 (タイプ指定 : 最初に見つかったポインタ) ---
//==================================================================================
CObject *CObject::FindAnyObjectByType(const TYPE type)
{
	CObject *pObject = nullptr;		// 見つかったオブジェクトへのポインタ

	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 全プライオリティを検索
		pObject = FindAnyObjectByType(nCntPriority, type);

		// ポインタがnullでなければ終了
		if (pObject != nullptr) break;
	}

	// 検索結果を返す
	return pObject;
}

//==================================================================================
// --- オブジェクトの検索処理 (優先順位, タイプ指定 : 最初に見つかったポインタ) ---
//==================================================================================
CObject *CObject::FindAnyObjectByType(const int nPriority, const TYPE type)
{ // 範囲外ならアサーション
	assert(nPriority >= 0 && nPriority < MAX_OBJPRIORITY);

	CObject *pObject = m_apTop[nPriority];		// オブジェクトへのポインタ

	while (pObject != nullptr)
	{ // オブジェクトへのポインタがnullではない間ループ
		CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ

		// タイプが同じだった場合、ループ終了
		if (pObject->GetType() == type) break;

		pObject = pObjectNext;		// ポインタを進める
	}

	// 検索結果を返す
	return pObject;
}

//==================================================================================
// --- オブジェクトの検索処理 (タイプ指定 : 見つかったポインタ全て) ---
//==================================================================================
std::vector<CObject*> CObject::FindObjectsByType(const TYPE type)
{
	std::vector<CObject*> vpObject;				// 見つかったオブジェクトへのポインタ

	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 全プライオリティを検索
		std::vector<CObject*> vpFind;		// 今回のプライオリティで見つかった配列

		// 検索
		vpFind = std::move(FindObjectsByType(nCntPriority, type));

		// 配列の要素を挿入
		vpObject.insert(vpObject.end(), vpFind.begin(), vpFind.end());
	}

	// 検索結果を返す
	return vpObject;
}

//==================================================================================
// --- オブジェクトの検索処理 (優先順位, タイプ指定 : 見つかったポインタ全て) ---
//==================================================================================
std::vector<CObject*> CObject::FindObjectsByType(const int nPriority, const TYPE type)
{ // 範囲外ならアサーション
	assert(nPriority >= 0 && nPriority < MAX_OBJPRIORITY);

	CObject *pObject = m_apTop[nPriority];		// オブジェクトへのポインタ
	std::vector<CObject*> vpObject;				// 見つかったオブジェクトへのポインタ

	while (pObject != nullptr)
	{ // オブジェクトへのポインタがnullではない間ループ
		CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ

		// タイプが同じだった場合、ポインタを追加
		if (pObject->GetType() == type) vpObject.push_back(pObject);

		pObject = pObjectNext;		// ポインタを進める
	}

	// 検索結果を返す
	return vpObject;
}

//==================================================================================
// --- 自身の解放処理 ---
//==================================================================================
void CObject::Release(void)
{ // 死亡フラグを立てる
	m_bDeath = true;

	// 死亡したことを通知
	NotifyAllWhenDeath();
}

//==================================================================================
// --- 自身のリスト追加処理 ---
//==================================================================================
void CObject::AddList(void)
{
	CObject *&rpTop = m_apTop[m_nPriority];		// 先頭オブジェクトへのポインタ参照
	CObject *&rpCur = m_apCur[m_nPriority];		// 最後尾オブジェクトへのポインタ参照

	if (rpTop == nullptr && rpCur == nullptr)
	{ // もしリストが空なら
		rpTop = this;		// 先頭に自分を保存
		rpCur = this;		// 最後尾に自分を保存
	}
	else
	{ // リストに一つでも存在すれば
		rpCur->m_pNext = this;	// 最後尾のオブジェクトの次のオブジェクトへのポインタに自分を保存
		m_pPrev = rpCur;		// 最後尾のポインタを前のオブジェクトへのポインタに保存
		rpCur = this;			// 最後尾を自分に変更
	}
}

//==================================================================================
// --- 自身のリスト削除処理 ---
//==================================================================================
void CObject::RemoveList(void)
{
	CObject *&rpTop = m_apTop[m_nPriority];		// 先頭オブジェクトへのポインタ参照
	CObject *&rpCur = m_apCur[m_nPriority];		// 最後尾オブジェクトへのポインタ参照

	if (m_pNext != nullptr)
	{ // 次のオブジェクトが存在すれば
		m_pNext->m_pPrev = m_pPrev;		// 前のオブジェクトへのポインタを自分の前のオブジェクトへのポインタに変更
	}

	if (m_pPrev != nullptr)
	{ // 前のオブジェクトが存在すれば
		m_pPrev->m_pNext = m_pNext;		// 次のオブジェクトへのポインタを自分の次のオブジェクトへのポインタに変更
	}

	if (rpTop == this)
	{ // 先頭が自分なら
		rpTop = m_pNext;		// 先頭を次のオブジェクトに変更
	}

	if (rpCur == this)
	{ // 最後尾が自分なら
		rpCur = m_pPrev;		// 最後尾を前のオブジェクトに変更
	}
}

//==================================================================================
// --- 自身の死亡を観察者へ通知する処理 ---
//==================================================================================
void CObject::NotifyAllWhenDeath(void)
{ // 自身が死んだことを通知
	NotifyAll(NOTIFY_WHEN_DEATH);
}