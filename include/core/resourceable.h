//==================================================================================
//
// リソース管理インターフェースのヘッダファイル [resourceable.h]
// Author : TENMA SAITO
//
//==================================================================================
#ifndef _RESOURCEABLE_H_		// インクルードガード
#define _RESOURCEABLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"

//**********************************************************************************
// *** リソース管理インターフェース ***
// リソースの型に依存しない操作をまとめる(管理クラスを一括で扱う用)
//**********************************************************************************
class IResourceable
{
public:
	static constexpr UINT INVALID_RESOURCE_ID = static_cast<UINT>(-1);		// リソースIDの無効値

	virtual ~IResourceable() = default;

	virtual void Load(void) = 0;									// 登録予定のリソースを一括読み込み
	virtual void Unload(void) = 0;									// 全リソースの破棄

	UINT Register(std::string_view path) { return RegisterImpl(path); }				// 即時読み込みして登録(戻り値:リソースID)
	UINT Register(const char *pPath) { return (pPath == nullptr) ? INVALID_RESOURCE_ID : Register(std::string_view(pPath)); }		// ポインタのnullを確認し即時読み込みして登録(戻り値:リソースID)
	virtual UINT PreRegister(std::string_view path) = 0;			// 読み込み予定として登録(戻り値:リソースID)
	static constexpr bool IsValid(const UINT uIdx) { return uIdx != INVALID_RESOURCE_ID; }	// 無効値でないか(範囲や読み込み状態は見ない)

protected:
	virtual UINT RegisterImpl(std::string_view path) = 0;		// 読み込み処理の実装関数

	IResourceable() = default;

	// インターフェース経由のコピーによるスライシング防止
	IResourceable(const IResourceable&) = delete;
	IResourceable &operator=(const IResourceable&) = delete;
};

//**********************************************************************************
// *** リソース型指定リソース管理インターフェース ***
//**********************************************************************************
template <class Resource>
class ITypedResourceable : public IResourceable
{
public:
	virtual Resource *GetAddress(UINT uIdx) = 0;							// リソースの取得(無効なIDならnullptr)

protected:
	ITypedResourceable() = default;
};
#endif
