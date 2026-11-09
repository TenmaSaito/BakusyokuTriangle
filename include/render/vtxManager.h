#pragma once

#include "core/main.h"
#include <deque>

class CVtxBuffManager final
{
public:
	struct VTXBUFF_INFO
	{
		LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
		UINT nNumVtx;		// 頂点数
		DWORD dwUsage;		// フラグ
		DWORD dwFVF;		// FVF
		D3DPOOL pool;		// プールタイプ
		bool bUse;			// 使用しているか
	};

	CVtxBuffManager() = default;
	~CVtxBuffManager() = default;

	UINT Create(const LPDIRECT3DDEVICE9 pDevice,
		const UINT length,
		const DWORD dwUsage,
		const DWORD dwFVF,
		const D3DPOOL pool) 
	{
		for (auto &vtxInfo : m_dVtxInfo)
		{

		}
	}

	LPDIRECT3DVERTEXBUFFER9 GetAddress(const UINT nIdx);
	void Return(const UINT nIdx);
	void Release(const UINT nIdx);
	void ReleaseAll(void);

private:
	std::deque<VTXBUFF_INFO> m_dVtxInfo;
};